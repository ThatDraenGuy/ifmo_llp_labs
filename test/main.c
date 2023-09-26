#include "public/prelude.h"
#include <stdio.h>

int create_and_insert() {
  struct database_manager *database_manager = database_manager_new();
  TRY(database_manager_ctor(database_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, "TEST_TABLE", 3);
  table_schema_add_column(schema, "TEST_INT", COLUMN_TYPE_INT32);
  table_schema_add_column(schema, "TEST_BOOL", COLUMN_TYPE_BOOL);
  table_schema_add_column(schema, "TEST_FLOAT", COLUMN_TYPE_FLOAT);

  struct i_statement *create_table =
      create_table_statement_ctor(create_table_statement_new(), schema);

  struct statement_result *create_table_result = statement_result_new();
  TRY(database_manager_execute_statement(database_manager, create_table,
                                         create_table_result));
  CATCH(error, {
    database_manager_destroy(database_manager);
    statement_destroy(create_table);
    statement_result_destroy(create_table_result);
    handle_error(error);
    return 1;
  })
  statement_destroy(create_table);
  statement_result_destroy(create_table_result);

  struct record *record = record_new();
  record_ctor(record);
  struct i_statement *insert =
      insert_statement_ctor(insert_statement_new(), "TEST_TABLE", record);
  struct statement_result *insert_result = statement_result_new();

  for (int32_t i = 0; i < 10; i++) {
    float val = (float)i / 0.17f;
    record_insert(record, "TEST_INT", i);
    record_insert(record, "TEST_BOOL", (bool)(i % 3 == 0));
    record_insert(record, "TEST_FLOAT", val);

    TRY(database_manager_execute_statement(database_manager, insert,
                                           insert_result));
    CATCH(error, {
      database_manager_destroy(database_manager);
      statement_destroy(insert);
      statement_result_destroy(insert_result);
      handle_error(error);
      return 1;
    })

    record_clear(record);
    statement_result_clear(insert_result);
  }
  statement_destroy(insert);
  statement_result_destroy(insert_result);
  database_manager_destroy(database_manager);

  printf("done");
  return 0;
}

int query() {
  struct database_manager *database_manager = database_manager_new();
  TRY(database_manager_ctor(database_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  struct predicate *predicate = predicate_of(
      column_value("TEST_BOOL", COLUMN_TYPE_BOOL), literal((bool)true), EQ);
  struct i_statement *query =
      query_statement_ctor(query_statement_new(), "TEST_TABLE", predicate);
  struct statement_result *query_result = statement_result_new();

  TRY(database_manager_execute_statement(database_manager, query,
                                         query_result));
  CATCH(error, {
    database_manager_destroy(database_manager);
    statement_destroy(query);
    statement_result_destroy(query_result);
    handle_error(error);
    return 1;
  })

  struct record_iterator *it = statement_result_records(query_result);
  statement_destroy(query);

  struct record *record = NULL;
  while (record_iterator_has_next(it)) {
    TRY(record_iterator_next(it, &record));
    CATCH(error, {
      database_manager_destroy(database_manager);
      statement_result_destroy(query_result);
      handle_error(error);
      return 1;
    })

    TRY(record_print(record));
    CATCH(error, {
      database_manager_destroy(database_manager);
      statement_result_destroy(query_result);
      handle_error(error);
      return 1;
    })
  }

  statement_result_destroy(query_result);
  database_manager_destroy(database_manager);

  printf("done");
  return 0;
}

int main() { return create_and_insert(); }
