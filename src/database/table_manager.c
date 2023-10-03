//
// Created by draen on 15.09.23.
//

#include "private/database/table_manager.h"
#include "private/database/domain/record/record_update.h"
#include "private/database/domain/record/record_view.h"
#include "private/database/domain/schema.h"
#include "private/database/domain/table.h"
#include "public/database/domain/expression/expression_operators.h"
#include "public/database/domain/expression/expressions.h"
#include "public/database/domain/predicate.h"
#include "public/database/domain/record/record.h"
#include "public/database/domain/record/single_record_holder.h"
#include "public/database/record_serialization.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <string.h>

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
  return page_data_manager_insert(self->page_data_manager, meta_group, item,
                                  true);
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
  table_schema_ctor(table_data_table_schema, TABLE_DATA_TABLE_NAME(), 4);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_TABLE_ID(),
                          COLUMN_TYPE_UINT64);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_TABLE_NAME(),
                          COLUMN_TYPE_STRING);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID(),
                          COLUMN_TYPE_UINT64);
  table_schema_add_column(table_data_table_schema,
                          TABLE_DATA_TABLE_COLUMN_COLUMNS_AMOUNT(),
                          COLUMN_TYPE_UINT64);

  struct table_schema *table_columns_table_schema = table_schema_new();
  table_schema_ctor(table_columns_table_schema, TABLE_COLUMNS_TABLE_NAME(), 3);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID(),
                          COLUMN_TYPE_UINT64);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_NAME(),
                          COLUMN_TYPE_STRING);
  table_schema_add_column(table_columns_table_schema,
                          TABLE_COLUMNS_TABLE_COLUMN_TYPE(),
                          COLUMN_TYPE_UINT64);

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
                                        struct single_record_holder **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  *result = single_record_holder_new();
  single_record_holder_ctor(*result, 1, table_get_schema(table));
  while (item_iterator_has_next(it)) {
    single_record_holder_clear_all(*result);
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(*result);
      THROW(error);
    })

    TRY(record_deserialize(
        item, single_record_holder_get((*result)), 0,
        table_schema_get_column_amount(table_get_schema(table))));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(*result);
      THROW(error);
    })

    bool predicate_result = false;
    TRY(predicate_apply(predicate, single_record_holder_get((*result)),
                        &predicate_result));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(*result);
      THROW(error);
    })
    if (predicate_result) {
      if (do_delete) {
        TRY(item_iterator_delete_item(it));
        CATCH(error, {
          item_iterator_destroy(it);
          single_record_holder_destroy(*result);
          THROW(error);
        })
        TRY(page_data_manager_vacuum(self->page_data_manager,
                                     table->page_group_id));
        CATCH(error, {
          item_iterator_destroy(it);
          single_record_holder_destroy(*result);
          THROW(error);
        })
      }
      item_iterator_destroy(it);
      OK;
    }
  }

  item_iterator_destroy(it);
  single_record_holder_destroy(*result);
  THROW(error_self(NO_RECORD_FOUND));
}

result_t table_manager_find_with_joins(
    struct table_manager *self, struct table *table,
    struct predicate *predicate, size_t joins_num, struct table **join_tables,
    struct predicate **join_predicates, struct record_view **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  *result = malloc(sizeof(struct record_view));
  (*result)->table_manager = self;
  (*result)->main_item_it = page_data_manager_get_items(self->page_data_manager,
                                                        table->page_group_id);
  (*result)->main_table = table;
  (*result)->is_empty = false;
  (*result)->where = predicate;
  (*result)->joins_num = joins_num;
  (*result)->join_tables = join_tables;
  (*result)->join_predicates = join_predicates;

  (*result)->next_record_holder = single_record_holder_new();
  single_record_holder_ctor_with_joins((*result)->next_record_holder, table,
                                       joins_num, join_tables);

  (*result)->current_record_holder = single_record_holder_new();
  single_record_holder_ctor_with_joins((*result)->current_record_holder, table,
                                       joins_num, join_tables);

  struct record *values = NULL;
  TRY(record_view_next(*result, &values));
  CATCH(error, {
    record_view_destroy(*result);
    THROW(error);
  })

  OK;
}

result_t table_manager_find(struct table_manager *self, struct table *table,
                            struct predicate *predicate,
                            struct record_view **result) {
  return table_manager_find_with_joins(self, table, predicate, 0, NULL, NULL,
                                       result);
}

result_t table_manager_find_one(struct table_manager *self, struct table *table,
                                struct predicate *predicate,
                                size_t column_offset,
                                struct single_record_holder *target) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  while (item_iterator_has_next(it)) {
    single_record_holder_clear(
        target, column_offset,
        table_schema_get_column_amount(table_get_schema(table)));
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      THROW(error);
    })

    TRY(record_deserialize(item, single_record_holder_get(target),
                           column_offset,
                           column_offset + table_schema_get_column_amount(
                                               table_get_schema(table))));
    CATCH(error, {
      item_iterator_destroy(it);
      THROW(error);
    })

    bool predicate_result = false;
    TRY(predicate_apply(predicate, single_record_holder_get(target),
                        &predicate_result));
    CATCH(error, {
      item_iterator_destroy(it);
      THROW(error);
    })
    if (predicate_result) {
      item_iterator_destroy(it);
      OK;
    }
  }

  item_iterator_destroy(it);
  THROW(error_self(NO_RECORD_FOUND));
}

static result_t insert_record(struct table_manager *self, struct table *table,
                              struct record *record, bool immediate) {

  item_t item = record_serialize(record);
  TRY(page_data_manager_insert(self->page_data_manager, table->page_group_id,
                               item, immediate));
  CATCH(error, {
    item_destroy(item);
    THROW(error);
  })
  item_destroy(item);
  OK;
}

result_t table_manager_insert(struct table_manager *self, struct table *table,
                              struct record_group *records) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct record_iterator *it = record_group_get_records(records);
  while (record_iterator_has_next(it)) {
    struct record *record = NULL;
    TRY(record_iterator_next(it, &record));
    CATCH(error, {
      record_iterator_destroy(it);
      THROW(error);
    })

    TRY(insert_record(self, table, record, true));
    CATCH(error, {
      record_iterator_destroy(it);
      THROW(error);
    })
  }
  record_iterator_destroy(it);

  OK;
}

result_t table_manager_update(struct table_manager *self, struct table *table,
                              struct predicate *predicate,
                              struct record_update *record_update) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct single_record_holder *record_holder = single_record_holder_new();
  single_record_holder_ctor(record_holder, 1, table_get_schema(table));
  while (item_iterator_has_next(it)) {
    single_record_holder_clear_all(record_holder);
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })

    TRY(record_deserialize(
        item, single_record_holder_get(record_holder), 0,
        table_schema_get_column_amount(table_get_schema(table))));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })

    bool predicate_res = false;
    TRY(predicate_apply(predicate, single_record_holder_get(record_holder),
                        &predicate_res));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })
    if (predicate_res) {
      TRY(record_update_apply(record_update,
                              single_record_holder_get(record_holder)));
      CATCH(error, {
        item_iterator_destroy(it);
        single_record_holder_destroy(record_holder);
        THROW(error);
      })

      if (record_update->applicable_in_place) {
        record_serialize_into(single_record_holder_get(record_holder), item);
      } else {
        item_iterator_delete_item(it);
        insert_record(self, table, single_record_holder_get(record_holder),
                      false);
      }
    }
  }
  single_record_holder_destroy(record_holder);
  item_iterator_destroy(it);
  page_data_manager_vacuum(self->page_data_manager, table->page_group_id);

  OK;
}

result_t table_manager_delete(struct table_manager *self, struct table *table,
                              struct predicate *predicate) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct item_iterator *it = page_data_manager_get_items(
      self->page_data_manager, table->page_group_id);

  struct single_record_holder *record_holder = single_record_holder_new();
  single_record_holder_ctor(record_holder, 1, table_get_schema(table));
  while (item_iterator_has_next(it)) {
    single_record_holder_clear_all(record_holder);
    item_t item = ITEM_NULL;
    TRY(item_iterator_next(it, &item));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })

    TRY(record_deserialize(
        item, single_record_holder_get(record_holder), 0,
        table_schema_get_column_amount(table_get_schema(table))));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })

    bool predicate_res = false;
    TRY(predicate_apply(predicate, single_record_holder_get(record_holder),
                        &predicate_res));
    CATCH(error, {
      item_iterator_destroy(it);
      single_record_holder_destroy(record_holder);
      THROW(error);
    })
    if (predicate_res) {
      TRY(item_iterator_delete_item(it));
      CATCH(error, {
        item_iterator_destroy(it);
        single_record_holder_destroy(record_holder);
        THROW(error);
      })
    }
  }

  single_record_holder_destroy(record_holder);
  item_iterator_destroy(it);
  return page_data_manager_vacuum(self->page_data_manager,
                                  table->page_group_id);
}

result_t table_manager_create_table(struct table_manager *self,
                                    struct table_schema *schema) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  // try to find a table with requested name
  struct predicate *table_name_equal = predicate_of(expr_of(
      column_expr(TABLE_DATA_TABLE_NAME(), TABLE_DATA_TABLE_COLUMN_TABLE_NAME(),
                  COLUMN_TYPE_STRING),
      literal_expr(table_schema_get_name(schema)),
      comparison_operator(EQ, COLUMN_TYPE_STRING)));

  bool does_table_exist = true;
  struct single_record_holder *existing_table_data_record = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              false, &existing_table_data_record));
  CATCH(error, {
    // if we encounter any error but NO_RECORD_FOUND, propagate main_item_it
    // otherwise everything is good
    if (!(strcmp(error_get_type(error), ERROR_TYPE) == 0 &&
          error_get_code(error).bytes == NO_RECORD_FOUND)) {
      predicate_destroy(table_name_equal);
      THROW(error);
    } else {
      does_table_exist = false;
    }
    error_destroy(error);
  })
  predicate_destroy(table_name_equal);

  // if a table with the same name already exists throw error
  if (does_table_exist) {
    single_record_holder_destroy(existing_table_data_record);
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
  struct record_group *table_data_group = record_group_new();
  record_group_ctor(table_data_group, 1, self->table_data_table->table_schema);

  record_group_insert(table_data_group, 4, COLUMN_VALUE(table_id),
                      COLUMN_VALUE(table_schema_get_name(schema)),
                      COLUMN_VALUE(table_group.bytes),
                      COLUMN_VALUE(table_schema_get_column_amount(schema)));

  // save table record to the tables table
  TRY(table_manager_insert(self, self->table_data_table, table_data_group));
  CATCH(error, {
    record_group_destroy(table_data_group);
    THROW(error);
  })
  record_group_destroy(table_data_group);

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
    struct record_group *column_data_group = record_group_new();
    record_group_ctor(column_data_group, 1,
                      self->table_columns_table->table_schema);
    record_group_insert(
        column_data_group, 3, COLUMN_VALUE(table_id),
        COLUMN_VALUE(column_schema_get_name(column_schema)),
        COLUMN_VALUE((uint64_t)column_schema_get_type(column_schema)));

    // save main_item_it to columns table
    TRY(table_manager_insert(self, self->table_columns_table,
                             column_data_group));
    CATCH(error, {
      column_schema_iterator_destroy(column_it);
      record_group_destroy(column_data_group);
      THROW(error);
    })
    record_group_destroy(column_data_group);
  }
  column_schema_iterator_destroy(column_it);

  OK;
}

result_t table_manager_get_table(struct table_manager *self, str_t table_name,
                                 struct table **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  // where to find table by name
  struct predicate *table_name_equal = predicate_of(expr_of(
      column_expr(TABLE_DATA_TABLE_NAME(), TABLE_DATA_TABLE_COLUMN_TABLE_NAME(),
                  COLUMN_TYPE_STRING),
      literal_expr(table_name), comparison_operator(EQ, COLUMN_TYPE_STRING)));

  // find table record
  struct single_record_holder *table_data_holder = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              false, &table_data_holder));
  CATCH(error, {
    predicate_destroy(table_name_equal);
    THROW(error);
  })
  predicate_destroy(table_name_equal);

  struct record *table_data = single_record_holder_get(table_data_holder);

  // extract table id from the record
  uint64_t table_id = 0;
  TRY(record_get(table_data, TABLE_DATA_TABLE_NAME(),
                 TABLE_DATA_TABLE_COLUMN_TABLE_ID(), &table_id));
  CATCH(error, {
    single_record_holder_destroy(table_data_holder);
    THROW(error);
  })

  // extract page group id from the record
  page_group_id_t table_page_group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get(table_data, TABLE_DATA_TABLE_NAME(),
                 TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID(),
                 &table_page_group_id.bytes));
  CATCH(error, {
    single_record_holder_destroy(table_data_holder);
    THROW(error);
  })

  // extract columns amount from the record
  uint64_t columns_amount = 0;
  TRY(record_get(table_data, TABLE_DATA_TABLE_NAME(),
                 TABLE_DATA_TABLE_COLUMN_COLUMNS_AMOUNT(), &columns_amount));
  CATCH(error, {
    single_record_holder_destroy(table_data_holder);
    THROW(error);
  })

  // don't need table record anymore
  single_record_holder_destroy(table_data_holder);

  // where to find columns by table id
  struct predicate *table_id_equal = predicate_of(expr_of(
      column_expr(TABLE_COLUMNS_TABLE_NAME(),
                  TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID(), COLUMN_TYPE_UINT64),
      literal_expr(table_id), comparison_operator(EQ, COLUMN_TYPE_UINT64)));

  // find the columns
  struct record_view *columns_view = NULL;
  TRY(table_manager_find(self, self->table_columns_table, table_id_equal,
                         &columns_view));
  CATCH(error, THROW(error))

  // start constructing table schema
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, table_name, columns_amount);

  struct record *column_data = NULL;
  while (record_view_has_next(columns_view)) {
    // get column record
    TRY(record_view_next(columns_view, &column_data));
    CATCH(error, {
      record_view_destroy(columns_view);
      table_schema_destroy(schema);
      THROW(error);
    })

    // extract column type from the record
    uint64_t column_type_index;
    TRY(record_get(column_data, TABLE_COLUMNS_TABLE_NAME(),
                   TABLE_COLUMNS_TABLE_COLUMN_TYPE(), &column_type_index));
    CATCH(error, {
      record_view_destroy(columns_view);
      table_schema_destroy(schema);
      THROW(error);
    })
    column_type_t column_type = column_type_index;

    // extract column name from the record
    str_t column_name;
    TRY(record_get(column_data, TABLE_COLUMNS_TABLE_NAME(),
                   TABLE_COLUMNS_TABLE_COLUMN_NAME(), &column_name));
    CATCH(error, {
      record_view_destroy(columns_view);
      table_schema_destroy(schema);
      THROW(error);
    })

    // add column data to the schema
    table_schema_add_column(schema, column_name, column_type);
  }
  record_view_destroy(columns_view);

  // finally construct table struct
  *result = table_new();
  table_ctor(*result, schema, table_page_group_id);
  OK;
}

result_t table_manager_drop_table(struct table_manager *self,
                                  str_t table_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct predicate *table_name_equal = predicate_of(expr_of(
      column_expr(TABLE_DATA_TABLE_NAME(), TABLE_DATA_TABLE_COLUMN_TABLE_NAME(),
                  COLUMN_TYPE_STRING),
      literal_expr(table_name), comparison_operator(EQ, COLUMN_TYPE_STRING)));

  struct single_record_holder *table_data_holder = NULL;
  TRY(find_first_maybe_delete(self, self->table_data_table, table_name_equal,
                              true, &table_data_holder));
  CATCH(error, {
    predicate_destroy(table_name_equal);
    THROW(error);
  })
  predicate_destroy(table_name_equal);

  struct record *table_data = single_record_holder_get(table_data_holder);

  uint64_t table_id = 0;
  TRY(record_get(table_data, TABLE_DATA_TABLE_NAME(),
                 TABLE_DATA_TABLE_COLUMN_TABLE_ID(), &table_id));
  CATCH(error, {
    single_record_holder_destroy(table_data_holder);
    THROW(error);
  })

  page_group_id_t group_id = PAGE_GROUP_ID_NULL;
  TRY(record_get(table_data, TABLE_DATA_TABLE_NAME(),
                 TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID(), &group_id.bytes));
  CATCH(error, {
    single_record_holder_destroy(table_data_holder);
    THROW(error);
  })
  single_record_holder_destroy(table_data_holder);

  struct predicate *column_delete = predicate_of(expr_of(
      column_expr(TABLE_COLUMNS_TABLE_NAME(),
                  TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID(), COLUMN_TYPE_UINT64),
      literal_expr(table_id), comparison_operator(EQ, COLUMN_TYPE_UINT64)));

  TRY(table_manager_delete(self, self->table_columns_table, column_delete));
  CATCH(error, {
    predicate_destroy(column_delete);
    THROW(error);
  })
  predicate_destroy(column_delete);

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