#include "public/prelude.h"
#include <stdio.h>

STR(TEST_TABLE_NAME, "TEST_TABLE")
STR(TEST_INT_COLUMN, "TEST_INT")
STR(TEST_BOOL_COLUMN, "TEST_BOOL")
STR(TEST_FLOAT_COLUMN, "TEST_FLOAT")

int create_and_insert(struct database_manager *database_manager) {
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, TEST_TABLE_NAME(), 3);
  table_schema_add_column(schema, TEST_INT_COLUMN(), COLUMN_TYPE_INT32);
  table_schema_add_column(schema, TEST_BOOL_COLUMN(), COLUMN_TYPE_BOOL);
  table_schema_add_column(schema, TEST_FLOAT_COLUMN(), COLUMN_TYPE_FLOAT);

  struct i_statement *create_table =
      create_table_statement_ctor(create_table_statement_new(), schema);

  struct statement_result *create_table_result = statement_result_new();
  TRY(database_manager_execute_statement(database_manager, create_table,
                                         create_table_result));
  CATCH(error, {
    statement_destroy(create_table);
    statement_result_destroy(create_table_result);
    table_schema_destroy(schema);
    handle_error(error);
    return 1;
  })
  statement_destroy(create_table);
  statement_result_destroy(create_table_result);

  struct record_group *record_group = record_group_new();
  record_group_ctor(record_group, 1, schema);
  for (int32_t i = 0; i < 10; i++) {
    record_group_insert(record_group, 3, COLUMN_VALUE(i),
                        COLUMN_VALUE((bool)(i % 3 == 0)),
                        COLUMN_VALUE((float)i / 3.f));
  }

  struct i_statement *insert = insert_statement_ctor(
      insert_statement_new(), TEST_TABLE_NAME(), record_group);
  struct statement_result *insert_result = statement_result_new();
  TRY(database_manager_execute_statement(database_manager, insert,
                                         insert_result));
  CATCH(error, {
    statement_destroy(insert);
    statement_result_destroy(insert_result);
    table_schema_destroy(schema);
    handle_error(error);
    return 1;
  })

  statement_destroy(insert);
  statement_result_destroy(insert_result);
  table_schema_destroy(schema);

  printf("done");
  return 0;
}

int query(struct database_manager *database_manager) {
  struct predicate *predicate =
      predicate_of(column_value(TEST_BOOL_COLUMN(), COLUMN_TYPE_BOOL),
                   literal((bool)false), EQ);

  struct i_statement *query =
      query_statement_ctor(query_statement_new(), TEST_TABLE_NAME(), predicate);
  struct statement_result *query_result = statement_result_new();

  TRY(database_manager_execute_statement(database_manager, query,
                                         query_result));
  CATCH(error, {
    statement_destroy(query);
    statement_result_destroy(query_result);
    handle_error(error);
    return 1;
  })

  struct record_view *view = statement_result_records(query_result);
  statement_destroy(query);
  schema_print(record_view_get_schema(view));

  struct record *record = NULL;
  while (record_view_has_next(view)) {
    TRY(record_view_next(view, &record));
    CATCH(error, {
      statement_result_destroy(query_result);
      handle_error(error);
      return 1;
    })

    TRY(record_print(record));
    CATCH(error, {
      statement_result_destroy(query_result);
      handle_error(error);
      return 1;
    })
  }

  statement_result_destroy(query_result);

  printf("done");
  return 0;
}

int main() {
  struct database_manager *database_manager = database_manager_new();
  TRY(database_manager_ctor(database_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  int res = create_and_insert(database_manager);

  database_manager_destroy(database_manager);
  return res;
}
