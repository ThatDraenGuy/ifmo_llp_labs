//
// Created by draen on 03.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "database//prelude.h"

#define TABLE_NAME_2 STR_OF("2Table")
#define ID_COL_2 STR_OF("ID")
#define REF_COL_2 STR_OF("REF")

static result_t second_table(struct database_manager *database_manager,
                             uint64_t record_num) {
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, TABLE_NAME_2, 2);
  table_schema_add_column(schema, ID_COL_2, COLUMN_TYPE_UINT64);
  table_schema_add_column(schema, REF_COL_2, COLUMN_TYPE_UINT64);

  struct statement_result *result = NULL;
  {
    struct i_statement *create = create_table_statement_of(schema);
    TRY(database_manager_execute_statement(database_manager, create, &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
  }
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema);

    for (uint64_t id = 0; id < record_num; id++) {
      uint64_t ref_id = record_num - 1 - id;
      record_group_insert(values, 2, COLUMN_VALUE(id), COLUMN_VALUE(ref_id));
    }

    struct i_statement *insert = insert_statement_of(TABLE_NAME_2, values);
    TRY(database_manager_execute_statement(database_manager, insert, &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
  }
  database_manager_finish_statement(database_manager);
  table_schema_destroy(schema);
  {
    struct predicate *where = predicate_of(LITERAL_TRUE);

    struct predicate *join_on = predicate_of(
        expr_of(column_expr(TABLE_NAME, ID_COL, COLUMN_TYPE_UINT64),
                column_expr(TABLE_NAME_2, REF_COL_2, COLUMN_TYPE_UINT64),
                comparison_operator(EQ, COLUMN_TYPE_UINT64)));

    struct i_statement *query = query_statement_of(
        TABLE_NAME_2, where, 1, join_of(TABLE_NAME, join_on));
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

static result_t test(struct database_manager *database_manager) {
  uint64_t record_num = 1000;
  TRY(create_common_table(database_manager));
  CATCH(error, THROW(error))

  TRY(populate_common_table(database_manager, record_num));
  CATCH(error, THROW(error))

  TRY(second_table(database_manager, record_num));
  CATCH(error, THROW(error))
  OK;
}

int main() { WRAP_WITH_DB("database", test); }
