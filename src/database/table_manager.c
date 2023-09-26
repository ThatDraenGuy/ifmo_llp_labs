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

#define ERROR_SOURCE "TABLE_MANAGER"
#define ERROR_TYPE "TABLE_MANAGER_ERROR"

enum error_code {
  META_PAGE_CONTENTS_MISSING,
  META_PAGE_CONTENTS_INVALID,
  NO_RECORD_FOUND,
  TABLE_ALREADY_EXISTS
};

static const char *const error_messages[] = {
    [META_PAGE_CONTENTS_MISSING] = "Meta information is missing!",
    [META_PAGE_CONTENTS_INVALID] = "Meta information is invalid!",
    [NO_RECORD_FOUND] = "No applicable record was found!",
    [TABLE_ALREADY_EXISTS] = "table with this name already exists!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

struct table_manager *table_manager_new() {
  return malloc(sizeof(struct table_manager));
}

static result_t initialize_meta_page(struct table_manager *self,
                                     struct meta_contents *meta_contents) {
  page_group_id_t meta_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager, &meta_group));
  CATCH(error, THROW(error))

  page_group_id_t table_data_table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager,
                                     &table_data_table_group));
  CATCH(error, THROW(error))

  page_group_id_t table_columns_table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager,
                                     &table_columns_table_group));
  CATCH(error, THROW(error))

  TRY(page_data_manager_set_meta_group_id(self->page_data_manager, meta_group));
  CATCH(error, THROW(error))

  meta_contents->table_data_table_page_group_id = table_data_table_group;
  meta_contents->table_columns_table_page_group_id = table_columns_table_group;
  meta_contents->next_table_id = 1;

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
    THROW(error_self(META_PAGE_CONTENTS_MISSING));
  }

  item_t item = ITEM_NULL;
  TRY(item_iterator_next(it, &item));
  CATCH(error, {
    item_iterator_destroy(it);
    THROW(error);
  })

  if (item.size != sizeof(struct meta_contents))
    THROW(error_self(META_PAGE_CONTENTS_INVALID));
  memcpy(meta_contents, item.data, item.size);

  item_iterator_destroy(it);
  OK;
}

static result_t get_next_table_id(struct table_manager *self,
                                  uint64_t *result) {
  page_group_id_t meta_group =
      page_data_manager_get_meta_group_id(self->page_data_manager);
  struct item_iterator *it =
      page_data_manager_get_items(self->page_data_manager, meta_group);

  if (!item_iterator_has_next(it)) {
    item_iterator_destroy(it);
    THROW(error_self(META_PAGE_CONTENTS_MISSING));
  }

  item_t meta_contents_item = ITEM_NULL;
  TRY(item_iterator_next(it, &meta_contents_item));
  CATCH(error, {
    item_iterator_destroy(it);
    THROW(error);
  })
  item_iterator_destroy(it);

  if (meta_contents_item.size != sizeof(struct meta_contents))
    THROW(error_self(META_PAGE_CONTENTS_INVALID));

  struct meta_contents *meta_contents = meta_contents_item.data;
  *result = meta_contents->next_table_id;
  meta_contents->next_table_id++; // TODO really think
  OK;
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
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct page_data_manager *page_data_manager = page_data_manager_new();

  TRY(page_data_manager_ctor(page_data_manager, file_name));
  CATCH(error, {
    free(self);
    THROW(error);
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
      THROW(error);
    })
  } else {
    TRY(get_data_from_meta_page(self, meta_group_id, meta_contents));
    CATCH(error, {
      page_data_manager_destroy(page_data_manager);
      free(meta_contents);
      free(self);
      THROW(error);
    })
  }
  initialize_meta_tables(self, meta_contents);

  free(meta_contents);
  OK;
}

static result_t find_first_maybe_delete(struct table_manager *self,
                                        struct table *table,
                                        struct predicate *predicate,
                                        bool do_delete,
                                        struct record **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct record *record = record_new();
  record_ctor(record);
  while (item_iterator_has_next(it)) {
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })

    TRY(record_deserialize(item, table->table_schema, record));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })

    bool predicate_result = false;
    TRY(predicate_apply(predicate, record, &predicate_result));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })
    if (predicate_result) {
      if (do_delete)
        item_iterator_delete_item(it);
      item_iterator_destroy(it);
      *result = record;
      OK;
    }
  }

  item_iterator_destroy(it);
  record_destroy(record);
  THROW(error_self(NO_RECORD_FOUND));
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
  TRY(record_copy_into(self->next_record, self->current_record));
  CATCH(error, THROW(error))

  *result = self->current_record;

  // try to find next applicable item
  while (item_iterator_has_next(self->item_it)) {
    record_clear(self->next_record);

    item_t new_item = ITEM_NULL;
    TRY(item_iterator_next(self->item_it, &new_item));
    CATCH(error, THROW(error))

    TRY(record_deserialize(new_item, self->table->table_schema,
                           self->next_record));
    CATCH(error, THROW(error))

    bool predicate_res = false;
    TRY(predicate_apply(self->predicate, self->next_record, &predicate_res));
    CATCH(error, THROW(error))
    if (predicate_res) {
      OK;
    }
  }
  self->is_empty = true;
  OK;
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
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *item_it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  *result = malloc(sizeof(struct record_iterator));
  (*result)->table = table;
  (*result)->item_it = item_it;
  (*result)->predicate = predicate;
  (*result)->is_empty = false;

  (*result)->current_record = record_new();
  record_ctor((*result)->current_record);
  (*result)->next_record = record_new();
  record_ctor((*result)->next_record);

  // preload first item (needed for predefined has_next calls)
  struct record *record = NULL;
  TRY(record_iterator_next(*result, &record));
  CATCH(error, {
    record_iterator_destroy(*result);
    THROW(error);
  })

  OK;
}

result_t table_manager_insert(struct table_manager *self, struct table *table,
                              struct record *record) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  item_t item = record_serialize(record);

  TRY(page_data_manager_insert(self->page_data_manager, table->page_group_id,
                               item));
  CATCH(error, {
    item_destroy(item);
    THROW(error);
  })

  item_destroy(item);
  OK;
}

result_t table_manager_delete(struct table_manager *self, struct table *table,
                              struct predicate *predicate) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct record *record = record_new();
  record_ctor(record);
  while (item_iterator_has_next(it)) {
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })

    TRY(record_deserialize(item, table->table_schema, record));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })

    bool predicate_res = false;
    TRY(predicate_apply(predicate, record, &predicate_res));
    CATCH(error, {
      item_iterator_destroy(it);
      record_destroy(record);
      THROW(error);
    })
    if (predicate_res)
      item_iterator_delete_item(it);
    record_clear(record);
  }

  item_iterator_destroy(it);
  return page_data_manager_flush(self->page_data_manager, table->page_group_id);
}

result_t table_manager_create_table(struct table_manager *self,
                                    struct table_schema *schema) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  // try to find a table with requested name
  struct predicate *table_name_equal = predicate_of(
      column_value(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, COLUMN_TYPE_STRING),
      literal(schema->table_name), EQ);

  struct record *existing_table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              false, &existing_table_data_record));
  CATCH(error, {
    // if we encounter any error but NO_RECORD_FOUND, propagate it
    // otherwise everything is good
    if (!(strcmp(error_get_type(error), ERROR_TYPE) == 0 &&
          error_get_code(error).bytes == NO_RECORD_FOUND)) {
      predicate_destroy(table_name_equal);
      THROW(error);
    }
    error_destroy(error);
  })
  predicate_destroy(table_name_equal);

  // if a table with the same name already exists throw error
  if (existing_table_data_record != NULL) {
    record_destroy(existing_table_data_record);
    THROW(error_self(TABLE_ALREADY_EXISTS));
  }

  // create page group for the page
  page_group_id_t table_group = PAGE_GROUP_ID_NULL;
  TRY(page_data_manager_create_group(self->page_data_manager, &table_group));
  CATCH(error, THROW(error))

  // get table id
  uint64_t table_id = 0;
  TRY(get_next_table_id(self, &table_id));
  CATCH(error, THROW(error))

  // construct the table record
  struct record *table_data_record = record_new();
  record_ctor(table_data_record);
  record_insert(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_ID, table_id);
  record_insert(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_NAME,
                table_schema_get_name(schema));
  record_insert(table_data_record, TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                table_group.bytes);

  // save table record to the tables table
  TRY(table_manager_insert(self, self->table_data_table, table_data_record));
  CATCH(error, {
    record_destroy(table_data_record);
    THROW(error);
  })
  record_destroy(table_data_record);

  // create a record for all columns of the table
  struct column_schema_iterator *column_it = table_schema_get_columns(schema);
  while (column_schema_iterator_has_next(column_it)) {
    // get next column schema
    struct column_schema *column_schema = NULL;
    TRY(column_schema_iterator_next(column_it, &column_schema));
    CATCH(error, {
      column_schema_iterator_destroy(column_it);
      THROW(error);
    })

    // construct column record
    struct record *column_record = record_new();
    record_ctor(column_record);
    record_insert(column_record, TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, table_id);
    record_insert(column_record, TABLE_COLUMNS_TABLE_COLUMN_NAME,
                  column_schema->name);
    record_insert(column_record, TABLE_COLUMNS_TABLE_COLUMN_TYPE,
                  (uint64_t)column_schema->type);

    // save it to columns table
    TRY(table_manager_insert(self, self->table_columns_table, column_record));
    CATCH(error, {
      column_schema_iterator_destroy(column_it);
      record_destroy(column_record);
      THROW(error);
    })
    record_destroy(column_record);
  }
  column_schema_iterator_destroy(column_it);

  OK;
}

result_t table_manager_get_table(struct table_manager *self, char *table_name,
                                 struct table **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  // predicate to find table by name
  struct predicate *table_name_equal = predicate_of(
      column_value(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, COLUMN_TYPE_STRING),
      literal(table_name), EQ);

  // find table record
  struct record *table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              false, &table_data_record));
  CATCH(error, {
    predicate_destroy(table_name_equal);
    THROW(error);
  })
  predicate_destroy(table_name_equal);

  // extract table id from the record
  uint64_t table_id = 0;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_ID,
                 &table_id));
  CATCH(error, {
    record_destroy(table_data_record);
    THROW(error);
  })

  // extract page group id from the record
  page_group_id_t table_page_group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                 &table_page_group_id.bytes));
  CATCH(error, {
    record_destroy(table_data_record);
    THROW(error);
  })
  // don't need table record anymore
  record_destroy(table_data_record);

  // predicate to find columns by table id
  struct predicate *table_id_equal = predicate_of(
      column_value(TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, COLUMN_TYPE_UINT64),
      literal(table_id), EQ);

  // find the columns
  struct record_iterator *columns_it = NULL;
  TRY(table_manager_find(self, self->table_columns_table, table_id_equal,
                         &columns_it));
  CATCH(error, THROW(error))

  // start constructing table schema
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, table_name, 10); // TODO fix this
  struct record *column_record = NULL;
  while (record_iterator_has_next(columns_it)) {
    // get column record
    TRY(record_iterator_next(columns_it, &column_record));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      THROW(error);
    })

    // extract column type from the record
    uint64_t column_type_index;
    TRY(record_get(column_record, TABLE_COLUMNS_TABLE_COLUMN_TYPE,
                   &column_type_index));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      THROW(error);
    })
    column_type_t column_type = column_type_index;

    // extract column name from the record
    char *column_name = NULL;
    TRY(record_get(column_record, TABLE_COLUMNS_TABLE_COLUMN_NAME,
                   &column_name));
    CATCH(error, {
      record_iterator_destroy(columns_it);
      table_schema_destroy(schema);
      THROW(error);
    })

    // add column data to the schema
    table_schema_add_column(schema, column_name, column_type);
  }
  record_iterator_destroy(columns_it);

  // finally construct table struct
  struct table *table = malloc(sizeof(struct table));
  table->table_schema = schema;
  table->page_group_id = table_page_group_id;
  *result = table;
  OK;
}

result_t table_manager_drop_table(struct table_manager *self,
                                  char *table_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct predicate *table_name_equal = predicate_of(
      column_value(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, COLUMN_TYPE_STRING),
      literal(table_name), EQ);

  struct record *table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              true, &table_data_record));
  CATCH(error, THROW(error))

  uint64_t table_id = 0;
  TRY(record_get(table_data_record, TABLE_DATA_TABLE_COLUMN_TABLE_ID,
                 &table_id));
  CATCH(error, {
    record_destroy(table_data_record);
    THROW(error);
  })

  page_group_id_t group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get_uint64(table_data_record,
                        TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID,
                        &group_id.bytes));
  CATCH(error, {
    record_destroy(table_data_record);

    THROW(error);
  })
  record_destroy(table_data_record);

  struct predicate *column_delete = predicate_of(
      column_value(TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, COLUMN_TYPE_UINT64),
      literal(table_id), EQ);

  TRY(table_manager_delete(self, self->table_columns_table, column_delete));
  CATCH(error, {
    predicate_destroy(column_delete);
    THROW(error);
  })

  TRY(page_data_manager_delete_group(self->page_data_manager, group_id));
  CATCH(error, THROW(error))
  OK;
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