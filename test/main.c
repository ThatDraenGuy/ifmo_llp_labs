#include "public/prelude.h"
#include <stdio.h>

STR(TABLE1_NAME, "TABLE1")
STR(TABLE1_COL_ID, "ID")
STR(TABLE1_COL_DIV, "DIV")
STR(TABLE1_COL_NUM, "NUM")

STR(TABLE2_NAME, "TABLE2")
STR(TABLE2_COL_ID, "ID")
STR(TABLE2_COL_REF, "TABLE1_ID")

result_t create(struct database_manager *database_manager,
                struct table_schema *schema) {
  struct i_statement *create_table =
      create_table_statement_ctor(create_table_statement_new(), schema);

  struct statement_result *create_table_result = statement_result_new();
  TRY(database_manager_execute_statement(database_manager, create_table,
                                         create_table_result));
  CATCH(error, {
    statement_destroy(create_table);
    statement_result_destroy(create_table_result);
    THROW(error);
  })
  statement_destroy(create_table);
  statement_result_destroy(create_table_result);

  OK;
}

result_t insert(struct database_manager *database_manager,
                struct table_schema *schema1, struct table_schema *schema2) {
  uint64_t records_num = 1000;
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema1);
    for (uint64_t i = 0; i < records_num; i++) {
      record_group_insert(values, 3, COLUMN_VALUE(i),
                          COLUMN_VALUE((bool)(i % 3 == 0)),
                          COLUMN_VALUE((float)i / 3.f));
    }

    struct i_statement *insert = insert_statement_ctor(
        insert_statement_new(), table_schema_get_name(schema1), values);
    struct statement_result *insert_result = statement_result_new();
    TRY(database_manager_execute_statement(database_manager, insert,
                                           insert_result));
    CATCH(error, {
      statement_destroy(insert);
      statement_result_destroy(insert_result);
      THROW(error);
    })
    statement_destroy(insert);
    statement_result_destroy(insert_result);
  }
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema2);
    for (uint64_t i = 0; i < records_num; i++) {
      record_group_insert(values, 2, COLUMN_VALUE(i),
                          COLUMN_VALUE(records_num - 1 - i));
    }

    struct i_statement *insert = insert_statement_ctor(
        insert_statement_new(), table_schema_get_name(schema2), values);
    struct statement_result *insert_result = statement_result_new();
    TRY(database_manager_execute_statement(database_manager, insert,
                                           insert_result));
    CATCH(error, {
      statement_destroy(insert);
      statement_result_destroy(insert_result);
      THROW(error);
    })
    statement_destroy(insert);
    statement_result_destroy(insert_result);
  }
  OK;
}

result_t query(struct database_manager *database_manager) {
  struct predicate *where = predicate_of(
      column_value(TABLE1_NAME(), TABLE1_COL_DIV(), COLUMN_TYPE_BOOL),
      literal((bool)(true)), NEQ);

  struct predicate *join_on = predicate_of(
      column_value(TABLE1_NAME(), TABLE1_COL_ID(), COLUMN_TYPE_UINT64),
      column_value(TABLE2_NAME(), TABLE2_COL_REF(), COLUMN_TYPE_UINT64), EQ);

  struct i_statement *query =
      query_statement_ctor(query_statement_new(), TABLE2_NAME(), where, 1,
                           join_of(TABLE1_NAME(), join_on));

  struct statement_result *query_result = statement_result_new();
  TRY(database_manager_execute_statement(database_manager, query,
                                         query_result));
  CATCH(error, {
    statement_destroy(query);
    statement_result_destroy(query_result);
    THROW(error);
  })

  struct record_view *view = statement_result_records(query_result);

  struct record *record = NULL;
  while (record_view_has_next(view)) {
    TRY(record_view_next(view, &record));
    CATCH(error, {
      statement_destroy(query);
      statement_result_destroy(query_result);
      THROW(error);
    })

    TRY(record_print(record));
    CATCH(error, {
      statement_destroy(query);
      statement_result_destroy(query_result);
      THROW(error);
    })
  }

  statement_destroy(query);
  statement_result_destroy(query_result);
  OK;
}

result_t create_and_insert(struct database_manager *database_manager) {
  struct table_schema *schema1 = table_schema_new();
  table_schema_ctor(schema1, TABLE1_NAME(), 3);
  table_schema_add_column(schema1, TABLE1_COL_ID(), COLUMN_TYPE_UINT64);
  table_schema_add_column(schema1, TABLE1_COL_DIV(), COLUMN_TYPE_BOOL);
  table_schema_add_column(schema1, TABLE1_COL_NUM(), COLUMN_TYPE_FLOAT);

  struct table_schema *schema2 = table_schema_new();
  table_schema_ctor(schema2, TABLE2_NAME(), 2);
  table_schema_add_column(schema2, TABLE2_COL_ID(), COLUMN_TYPE_UINT64);
  table_schema_add_column(schema2, TABLE2_COL_REF(), COLUMN_TYPE_UINT64);

  TRY(create(database_manager, schema1));
  CATCH(error, {
    table_schema_destroy(schema1);
    table_schema_destroy(schema2);
    THROW(error);
  })

  TRY(create(database_manager, schema2));
  CATCH(error, {
    table_schema_destroy(schema1);
    table_schema_destroy(schema2);
    THROW(error);
  })

  TRY(insert(database_manager, schema1, schema2));
  CATCH(error, {
    table_schema_destroy(schema1);
    table_schema_destroy(schema2);
    THROW(error);
  })

  table_schema_destroy(schema1);
  table_schema_destroy(schema2);
  OK;
}

int main() {
  struct database_manager *database_manager = database_manager_new();
  TRY(database_manager_ctor(database_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  TRY(create_and_insert(database_manager));
  //    TRY(query(database_manager));
  CATCH(error, {
    database_manager_destroy(database_manager);
    handle_error(error);
    return 1;
  })

  database_manager_destroy(database_manager);
  printf("done");
  return 0;
}
