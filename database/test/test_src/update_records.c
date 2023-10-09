//
// Created by draen on 03.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "public/prelude.h"
#include <stdio.h>

static result_t test(struct database_manager *database_manager) {
  uint64_t record_num = 1000;
  TRY(create_common_table(database_manager));
  CATCH(error, THROW(error))

  TRY(populate_common_table(database_manager, record_num));
  CATCH(error, THROW(error))

  struct statement_result *result = NULL;
  {
    struct predicate *where = predicate_of(LITERAL_TRUE);
    struct i_statement *query = query_statement_of(TABLE_NAME(), where, 0);
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
  printf("--------------------------");
  {
    struct predicate *where = predicate_of(expr_of(
        column_expr(TABLE_NAME(), STATUS_COL(), COLUMN_TYPE_BOOL),
        literal_expr((bool)true), comparison_operator(EQ, COLUMN_TYPE_BOOL)));

    struct record_update *values = record_update_new();
    record_update_ctor(
        values, TABLE_NAME(), 1, NUM_COL(),
        expr_of(column_expr(TABLE_NAME(), NUM_COL(), COLUMN_TYPE_FLOAT),
                literal_expr(-1.f),
                arithmetic_operator(MUL, COLUMN_TYPE_FLOAT)));

    struct i_statement *update =
        update_statement_of(TABLE_NAME(), values, where);

    TRY(database_manager_execute_statement(database_manager, update, &result));
    CATCH(error, THROW(error))
  }

  {
    struct predicate *where = predicate_of(LITERAL_TRUE);
    struct i_statement *query = query_statement_of(TABLE_NAME(), where, 0);
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

int main() { WRAP_WITH_DB("database", test); }