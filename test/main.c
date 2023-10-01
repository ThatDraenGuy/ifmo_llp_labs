#include "public/database/domain/record_update.h"
#include "public/prelude.h"
#include <stdio.h>

STR(TABLE1_NAME, "TABLE1")
STR(TABLE1_COL_ID, "ID")
STR(TABLE1_COL_DIV, "DIV")
STR(TABLE1_COL_NUM, "NUM")

STR(TABLE2_NAME, "TABLE2")
STR(TABLE2_COL_ID, "ID")
STR(TABLE2_COL_REF, "TABLE1_ID")

STR(TABLE3_NAME, "TABLE3")
STR(TABLE3_COL_NUM, "NUM")

STR(TABLE4_NAME, "TABLE4")
STR(TABLE4_COL_ID, "ID")
STR(TABLE4_COL_NAME, "NAME")
STR(TABLE4_COL_VAL, "VAL")

STR(TABLE4_STR1, "amogus")
STR(TABLE4_STR2, "aboba")
STR(TABLE4_STR3, "never gonna give you up")
STR(TABLE4_STR4, "whose eyes are those eyes")

result_t test_table_with_strings(struct database_manager *database_manager) {
  struct statement_result *result = NULL;
  {
    struct table_schema *schema = table_schema_new();
    table_schema_ctor(schema, TABLE4_NAME(), 3);
    table_schema_add_column(schema, TABLE4_COL_ID(), COLUMN_TYPE_UINT64);
    table_schema_add_column(schema, TABLE4_COL_NAME(), COLUMN_TYPE_STRING);
    table_schema_add_column(schema, TABLE4_COL_VAL(), COLUMN_TYPE_FLOAT);

    struct i_statement *create_table = create_table_statement_of(schema);
    TRY(database_manager_execute_statement(database_manager, create_table,
                                           &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })

    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema);
    for (uint64_t id = 0; id < 500; id++) {
      str_t str = id % 3 == 0   ? TABLE4_STR1()
                  : id % 3 == 1 ? TABLE4_STR2()
                                : TABLE4_STR3();
      float val = 0.17f / (float)id * (id % 3 == 0 ? -1.f : 1.f);
      record_group_insert(values, 3, COLUMN_VALUE(id), COLUMN_VALUE(str),
                          COLUMN_VALUE(val));
    }
    struct i_statement *insert = insert_statement_of(TABLE4_NAME(), values);
    TRY(database_manager_execute_statement(database_manager, insert, &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
    database_manager_finish_statement(database_manager);
    table_schema_destroy(schema);
  }
  {
    struct record_update *values = record_update_new();
    record_update_ctor(values, TABLE4_NAME(), 2, TABLE4_COL_NAME(),
                       COLUMN_VALUE(TABLE4_STR4()), TABLE4_COL_VAL(),
                       COLUMN_VALUE((float)1.));
    struct predicate *where = predicate_of(
        column_value(TABLE4_NAME(), TABLE4_COL_VAL(), COLUMN_TYPE_FLOAT),
        literal((float)0.f), LESS);
    struct i_statement *update =
        update_statement_of(TABLE4_NAME(), values, where);
    TRY(database_manager_execute_statement(database_manager, update, &result));
    CATCH(error, THROW(error))
  }
  {
    struct predicate *where =
        predicate_of(literal((bool)true), literal((bool)true), EQ);
    struct i_statement *query = query_statement_of(TABLE4_NAME(), where, 0);
    TRY(database_manager_execute_statement(database_manager, query, &result));
    CATCH(error, THROW(error))

    struct record_view *view = statement_result_records(result);
    struct record *record = NULL;
    while (record_view_has_next(view)) {
      TRY(record_view_next(view, &record));
      CATCH(error, THROW(error))

      TRY(record_print(record));
      CATCH(error, THROW(error))
    }
  }
  OK;
}

result_t create(struct database_manager *database_manager,
                struct table_schema *schema) {
  struct i_statement *create_table = create_table_statement_of(schema);

  struct statement_result *create_table_result = NULL;
  TRY(database_manager_execute_statement(database_manager, create_table,
                                         &create_table_result));
  CATCH(error, THROW(error))
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

    struct i_statement *insert =
        insert_statement_of(table_schema_get_name(schema1), values);
    struct statement_result *insert_result = NULL;
    TRY(database_manager_execute_statement(database_manager, insert,
                                           &insert_result));
    CATCH(error, THROW(error))
  }
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema2);
    for (uint64_t i = 0; i < records_num; i++) {
      record_group_insert(values, 2, COLUMN_VALUE(i),
                          COLUMN_VALUE(records_num - 1 - i));
    }

    struct i_statement *insert =
        insert_statement_of(table_schema_get_name(schema2), values);
    struct statement_result *insert_result = NULL;
    TRY(database_manager_execute_statement(database_manager, insert,
                                           &insert_result));
    CATCH(error, THROW(error))
  }
  OK;
}

result_t query_table1(struct database_manager *database_manager) {
  struct predicate *where =
      predicate_of(literal((bool)true), literal((bool)true), EQ);
  struct i_statement *query = query_statement_of(TABLE1_NAME(), where, 0);
  struct statement_result *query_result = NULL;
  TRY(database_manager_execute_statement(database_manager, query,
                                         &query_result));
  CATCH(error, THROW(error))

  struct record_view *view = statement_result_records(query_result);
  struct record *record = NULL;
  while (record_view_has_next(view)) {
    TRY(record_view_next(view, &record));
    CATCH(error, THROW(error))

    TRY(record_print(record));
    CATCH(error, THROW(error))
  }
  OK;
}

result_t update(struct database_manager *database_manager) {
  struct record_update *values = record_update_new();
  record_update_ctor(values, TABLE1_NAME(), 1, TABLE1_COL_DIV(),
                     COLUMN_VALUE((bool)true));
  struct predicate *where = predicate_of(
      column_value(TABLE1_NAME(), TABLE1_COL_NUM(), COLUMN_TYPE_FLOAT),
      literal((float)100), GREATER);

  struct i_statement *update =
      update_statement_of(TABLE1_NAME(), values, where);
  struct statement_result *update_result = NULL;
  TRY(database_manager_execute_statement(database_manager, update,
                                         &update_result));
  CATCH(error, THROW(error))
  OK;
}

result_t query(struct database_manager *database_manager) {
  struct predicate *where = predicate_of(
      column_value(TABLE1_NAME(), TABLE1_COL_DIV(), COLUMN_TYPE_BOOL),
      literal((bool)(true)), NEQ);

  struct predicate *join_on = predicate_of(
      column_value(TABLE1_NAME(), TABLE1_COL_ID(), COLUMN_TYPE_UINT64),
      column_value(TABLE2_NAME(), TABLE2_COL_REF(), COLUMN_TYPE_UINT64), EQ);

  struct i_statement *query = query_statement_of(
      TABLE2_NAME(), where, 1, join_of(TABLE1_NAME(), join_on));

  struct statement_result *query_result = NULL;
  TRY(database_manager_execute_statement(database_manager, query,
                                         &query_result));
  CATCH(error, THROW(error))

  struct record_view *view = statement_result_records(query_result);

  struct record *record = NULL;
  while (record_view_has_next(view)) {
    TRY(record_view_next(view, &record));
    CATCH(error, THROW(error))

    TRY(record_print(record));
    CATCH(error, THROW(error))
  }
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

result_t replace(struct database_manager *database_manager) {
  {
    struct i_statement *drop = drop_table_statement_of(TABLE2_NAME());
    struct statement_result *drop_result = NULL;
    TRY(database_manager_execute_statement(database_manager, drop,
                                           &drop_result));
    CATCH(error, THROW(error))
  }
  {
    struct table_schema *schema = table_schema_new();
    table_schema_ctor(schema, TABLE3_NAME(), 1);
    table_schema_add_column(schema, TABLE3_COL_NUM(), COLUMN_TYPE_INT32);
    TRY(create(database_manager, schema));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })

    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema);
    for (int32_t i = -300; i < 300; i++) {
      record_group_insert(values, 1, COLUMN_VALUE(i));
    }
    struct i_statement *insert = insert_statement_of(TABLE3_NAME(), values);
    struct statement_result *insert_result = NULL;
    TRY(database_manager_execute_statement(database_manager, insert,
                                           &insert_result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
    table_schema_destroy(schema);
  }
  {
    struct predicate *delete_where = predicate_of(
        column_value(TABLE3_NAME(), TABLE3_COL_NUM(), COLUMN_TYPE_INT32),
        literal((int32_t)-10), GEQ);
    struct i_statement *delete =
        delete_statement_of(TABLE3_NAME(), delete_where);
    struct statement_result *delete_result = NULL;
    TRY(database_manager_execute_statement(database_manager, delete,
                                           &delete_result));
    CATCH(error, THROW(error))
  }
  {
    struct predicate *query_where =
        predicate_of(literal((bool)true), literal((bool)true), EQ);
    struct i_statement *query =
        query_statement_of(TABLE3_NAME(), query_where, 0);
    struct statement_result *query_result = NULL;
    TRY(database_manager_execute_statement(database_manager, query,
                                           &query_result));
    CATCH(error, THROW(error))
    struct record_view *view = statement_result_records(query_result);
    struct record *record = NULL;
    while (record_view_has_next(view)) {
      TRY(record_view_next(view, &record));
      CATCH(error, THROW(error))

      TRY(record_print(record));
      CATCH(error, THROW(error))
    }
  }

  OK;
}

int main() {
  struct database_manager *database_manager = database_manager_new();
  TRY(database_manager_ctor(database_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  TRY(test_table_with_strings(database_manager));
  CATCH(error, {
    database_manager_destroy(database_manager);
    handle_error(error);
    return 1;
  })

  database_manager_destroy(database_manager);
  printf("done");
  return 0;
}
