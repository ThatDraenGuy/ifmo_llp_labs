//
// Created by draen on 15.09.23.
//

#include "private/database/table_manager.h"
#include "private/database/domain/record.h"
#include "private/database/domain/table.h"
#include "public/database/record_serialization.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

static const char *const error_source = "TABLE_MANAGER";
static const char *const error_type = "TABLE_MANAGER_ERROR";

enum error_code {
  META_PAGE_CONTENTS_MISSING,
  META_PAGE_CONTENTS_INVALID,
  NO_RECORD_FOUND
};

static const char *const error_messages[] = {
    [META_PAGE_CONTENTS_MISSING] = "Meta information is missing!",
    [META_PAGE_CONTENTS_INVALID] = "Meta information is invalid!",
    [NO_RECORD_FOUND] = "No applicable record was found!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(error_source, error_type, (error_code_t){error_code},
                   error_messages[error_code]);
}

struct table_manager *table_manager_new() {
  return malloc(sizeof(struct table_manager));
}

static result_t initialize_meta_page(struct table_manager *self,
                                     struct meta_contents *meta_contents) {
  page_group_id_t meta_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager, &meta_group));
  CATCH(error, PROPAGATE)

  page_group_id_t table_data_table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager,
                                     &table_data_table_group));
  CATCH(error, PROPAGATE)

  page_group_id_t table_columns_table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager,
                                     &table_columns_table_group));
  CATCH(error, PROPAGATE)

  TRY(page_data_manager_set_meta_group_id(self->page_data_manager, meta_group));
  CATCH(error, PROPAGATE)

  meta_contents->table_data_table_page_group_id = table_data_table_group;
  meta_contents->table_columns_table_page_group_id = table_columns_table_group;

  item_t item =
      (item_t){.size = sizeof(struct meta_contents), .data = meta_contents};
  return page_data_manager_insert(self->page_data_manager, meta_group, item);
}

static result_t get_data_from_meta_page(struct table_manager *self,
                                        page_group_id_t meta_page_group_id,
                                        struct meta_contents *meta_contents) {
  struct item_iterator *it =
      page_data_manager_get_items(self->page_data_manager, meta_page_group_id);

  if (!item_iterator_has_next(it)) {
    return result_err(error_self(META_PAGE_CONTENTS_MISSING));
  }

  item_t item = ITEM_NULL;
  TRY(item_iterator_next(it, &item));
  CATCH(error, {
    item_iterator_destroy(it);
    PROPAGATE;
  })

  if (item.size != sizeof(struct meta_contents))
    return result_err(error_self(META_PAGE_CONTENTS_INVALID));
  memcpy(meta_contents, item.data, item.size);

  item_iterator_destroy(it);
  return OK;
}

static void initialize_meta_tables(struct table_manager *self,
                                   struct meta_contents *meta_contents) {
  struct table_schema *table_data_table_schema = table_schema_new();
  table_schema_ctor(table_data_table_schema, TABLE_DATA_TABLE_NAME, 3);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_TABLE_ID, COLUMN_TYPE_UINT64);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_TABLE_NAME,
                          COLUMN_TYPE_STRING);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                          COLUMN_TYPE_UINT64);

  struct table_schema *table_columns_table_schema = table_schema_new();
  table_schema_ctor(table_columns_table_schema, TABLE_COLUMNS_TABLE_NAME, 3);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID,
                          COLUMN_TYPE_UINT64);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_NAME, COLUMN_TYPE_STRING);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_TYPE, COLUMN_TYPE_UINT64);

  self->table_data_table = table_new();
  table_ctor(self->table_data_table, table_data_table_schema,
             meta_contents->table_data_table_page_group_id);

  self->table_columns_table = table_new();
  table_ctor(self->table_columns_table, table_columns_table_schema,
             meta_contents->table_columns_table_page_group_id);
}

result_t table_manager_ctor(struct table_manager *self, char *file_name) {
  ASSERT_NOT_NULL(self, error_source);

  struct page_data_manager *page_data_manager = page_data_manager_new();

  TRY(page_data_manager_ctor(page_data_manager, file_name));
  CATCH(error, {
    free(self);
    PROPAGATE;
  })
  self->page_data_manager = page_data_manager;

  page_group_id_t meta_group_id =
      page_data_manager_get_meta_group_id(self->page_data_manager);

  struct meta_contents *meta_contents = malloc(sizeof(struct meta_contents));
  if (page_group_id_is_null(meta_group_id)) {
    TRY(initialize_meta_page(self, meta_contents));
    CATCH(error, {
      page_data_manager_destroy(page_data_manager);
      free(meta_contents);
      free(self);
      PROPAGATE;
    })
  } else {
    TRY(get_data_from_meta_page(self, meta_group_id, meta_contents));
    CATCH(error, {
      page_data_manager_destroy(page_data_manager);
      free(meta_contents);
      free(self);
      PROPAGATE;
    })
  }
  initialize_meta_tables(self, meta_contents);

  free(meta_contents);
  return OK;
}

static result_t find_first_maybe_delete(struct table_manager *self,
                                        struct table *table,
                                        struct predicate *predicate,
                                        bool do_delete,
                                        struct record **result) {
  ASSERT_NOT_NULL(self, error_source);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct record *record = record_new();
  record_ctor(record, table_schema_get_column_amount(table->table_schema));
  while (item_iterator_has_next(it)) {
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      PROPAGATE;
    })

    TRY(record_deserialize(item, table->table_schema, record));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      PROPAGATE;
    })

    bool predicate_result = false;
    TRY(predicate_apply(predicate, record, &predicate_result));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      PROPAGATE;
    })
    if (predicate_result) {
      if (do_delete)
        item_iterator_delete_item(it);
      item_iterator_destroy(it);
      *result = record;
      return OK;
    }
  }

  item_iterator_destroy(it);
  return result_err(error_self(NO_RECORD_FOUND));
}

result_t table_manager_find_first(struct table_manager *self,
                                  struct table *table,
                                  struct predicate *predicate,
                                  struct record **result) {
  return find_first_maybe_delete(self, table, predicate, false, result);
}

bool record_iterator_has_next(struct record_iterator *self) {
  return !self->is_empty;
}

result_t record_iterator_next(struct record_iterator *self,
                              struct record **result) {
  record_clear(self->current_record);
  record_copy_into(self->next_record, self->current_record);
  *result = self->current_record;

  // try to find next applicable item
  while (item_iterator_has_next(self->item_it)) {
    record_clear(self->next_record);

    item_t new_item = ITEM_NULL;
    TRY(item_iterator_next(self->item_it, &new_item));
    CATCH(error, PROPAGATE)

    TRY(record_deserialize(new_item, self->table_schema, self->next_record));
    CATCH(error, PROPAGATE)

    bool predicate_res = false;
    TRY(predicate_apply(self->predicate, self->next_record, &predicate_res));
    CATCH(error, PROPAGATE)
    if (predicate_res) {
      return OK;
    }
  }
  self->is_empty = true;
  return OK;
}

void record_iterator_destroy(struct record_iterator *self) {
  predicate_destroy(self->predicate);
  item_iterator_destroy(self->item_it);
  record_destroy(self->current_record);
  record_destroy(self->next_record);
  free(self);
}

result_t table_manager_find(struct table_manager *self, struct table *table,
                            struct predicate *predicate,
                            struct record_iterator **result) {
  ASSERT_NOT_NULL(self, error_source);

  struct item_iterator *item_it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  *result = malloc(sizeof(struct record_iterator));
  (*result)->table_schema = table->table_schema;
  (*result)->item_it = item_it;
  (*result)->predicate = predicate;

  (*result)->current_record = record_new();
  record_ctor((*result)->current_record,
              table_schema_get_column_amount(table->table_schema));
  (*result)->next_record = record_new();
  record_ctor((*result)->next_record,
              table_schema_get_column_amount(table->table_schema));

  // preload first item (needed for predefined has_next calls)
  struct record *record = NULL;
  TRY(record_iterator_next(*result, &record));
  CATCH(error, {
    record_iterator_destroy(*result);
    PROPAGATE;
  })

  return OK;
}

result_t table_manager_insert(struct table_manager *self, struct table *table,
                              struct record *record) {
  ASSERT_NOT_NULL(self, error_source);

  item_t item = record_serialize(record);

  TRY(page_data_manager_insert(self->page_data_manager, table->page_group_id,
                               item));
  CATCH(error, {
    item_destroy(item);
    PROPAGATE;
  })

  item_destroy(item);
  return OK;
}

result_t table_manager_delete(struct table_manager *self, struct table *table,
                              struct predicate *predicate) {
  ASSERT_NOT_NULL(self, error_source);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct record *record = record_new();
  record_ctor(record, table_schema_get_column_amount(table->table_schema));
  while (item_iterator_has_next(it)) {
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      PROPAGATE;
    })

    TRY(record_deserialize(item, table->table_schema, record));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      PROPAGATE;
    })

    bool predicate_res = false;
    TRY(predicate_apply(predicate, record, &predicate_res));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      PROPAGATE;
    })
    if (predicate_res)
      item_iterator_delete_item(it);
    record_clear(record);
  }

  item_iterator_destroy(it);
  return page_data_manager_flush(self->page_data_manager, table->page_group_id);
}

result_t table_manager_create_table(struct table_manager *self,
                                    struct table_schema *schema,
                                    struct table **result) {
  ASSERT_NOT_NULL(self, error_source);

  page_group_id_t table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager, &table_group));
  CATCH(error, PROPAGATE)

  struct record *table_data_record = record_new();
  record_ctor(table_data_record, 3);

  // TODO table id
  record_insert(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_NAME,
                table_schema_get_name(schema));
  record_insert(table_data_record, TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                table_group.bytes);

  TRY(table_manager_insert(self, self->table_data_table, table_data_record));
  CATCH(error, {
    record_destroy(table_data_record);
    PROPAGATE;
  })
  record_destroy(table_data_record);

  struct column_schema_iterator *column_it = table_schema_get_columns(schema);
  while (column_schema_iterator_has_next(column_it)) {
    struct column_schema *column_schema = NULL;
    TRY(column_schema_iterator_next(column_it, &column_schema));
    CATCH(error, {
      column_schema_iterator_destroy(column_it);
      PROPAGATE;
    })

    struct record *column_record = record_new();
    record_ctor(column_record, 3);

    // TODO table_id
    record_insert(column_record, TABLE_COLUMNS_TABLE_COLUMN_TYPE,
                  (uint64_t)column_schema->type);
    record_insert(column_record, TABLE_COLUMNS_TABLE_COLUMN_NAME,
                  column_schema->name);

    TRY(table_manager_insert(self, self->table_columns_table, column_record));
    CATCH(error, {
      column_schema_iterator_destroy(column_it);
      record_destroy(column_record);
      PROPAGATE;
    })
    record_destroy(column_record);
  }
  column_schema_iterator_destroy(column_it);

  (*result) = table_new();
  table_ctor((*result), schema, table_group);

  return OK;
}

result_t table_manager_get_table(struct table_manager *self, char *table_name,
                                 struct table **result) {
  ASSERT_NOT_NULL(self, error_source);

  struct predicate *table_name_equal = predicate_of(
      column_value(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, COLUMN_TYPE_STRING),
      literal(table_name), EQ);

  struct record *table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              false, &table_data_record));
  CATCH(error, {
    predicate_destroy(table_name_equal);
    PROPAGATE;
  })
  predicate_destroy(table_name_equal);

  uint64_t table_id = 0;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_ID,
                 &table_id));
  CATCH(error, {
    record_destroy(table_data_record);
    PROPAGATE;
  })

  page_group_id_t table_page_group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                 &table_page_group_id.bytes));
  CATCH(error, {
    record_destroy(table_data_record);
    PROPAGATE;
  })
  record_destroy(table_data_record);

  struct predicate *table_id_equal = predicate_of(
      column_value(TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, COLUMN_TYPE_UINT64),
      literal(table_id), EQ);

  struct record_iterator *columns_it = NULL;
  TRY(table_manager_find(self, self->table_columns_table, table_id_equal,
                         &columns_it));
  CATCH(error, PROPAGATE)

  struct table_schema *schema = table_schema_new();
  while (record_iterator_has_next(columns_it)) {
    struct record *column_record = NULL;
    TRY(record_iterator_next(columns_it, &column_record));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      PROPAGATE;
    })

    uint64_t column_type_index;
    TRY(record_get(column_record, TABLE_COLUMNS_TABLE_COLUMN_TYPE,
                   &column_type_index));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      PROPAGATE;
    })
    column_type_t column_type = column_type_index;

    char *column_name = NULL;
    TRY(record_get(column_record, TABLE_COLUMNS_TABLE_COLUMN_NAME,
                   &column_name));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      PROPAGATE;
    })

    table_schema_add_column(schema, column_name, column_type);
  }
  record_iterator_destroy(columns_it);

  struct table *table = malloc(sizeof(struct table));
  table->table_schema = schema;
  table->page_group_id = table_page_group_id;
  *result = table;
  return OK;
}

result_t table_manager_drop_table(struct table_manager *self,
                                  char *table_name) {
  ASSERT_NOT_NULL(self, error_source);

  struct predicate *table_name_equal = predicate_of(
      column_value(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, COLUMN_TYPE_STRING),
      literal(table_name), EQ);

  struct record *table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              true, &table_data_record));
  CATCH(error, PROPAGATE)

  uint64_t table_id = 0;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_ID,
                 &table_id));
  CATCH(error, {
    record_destroy(table_data_record);
    PROPAGATE;
  })

  page_group_id_t group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get_uint64(table_data_record,
                        TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                        &group_id.bytes));
  CATCH(error, {
    record_destroy(table_data_record);

    PROPAGATE;
  })
  record_destroy(table_data_record);

  struct predicate *column_delete = predicate_of(
      column_value(TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, COLUMN_TYPE_UINT64),
      literal(table_id), EQ);

  TRY(table_manager_delete(self, self->table_columns_table, column_delete));
  CATCH(error, {
    predicate_destroy(column_delete);
    PROPAGATE;
  })

  TRY(page_data_manager_delete_group(self->page_data_manager, group_id));
  CATCH(error, PROPAGATE)
  return OK;
}

result_t table_manager_flush(struct table_manager *self, struct table *table) {
  return page_data_manager_flush(self->page_data_manager, table->page_group_id);
}

void table_manager_destroy(struct table_manager *self) {
  page_data_manager_destroy(self->page_data_manager);
  table_destroy(self->table_data_table);
  table_destroy(self->table_columns_table);
  free(self);
}