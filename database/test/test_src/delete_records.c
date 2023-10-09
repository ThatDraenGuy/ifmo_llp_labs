//
// Created by draen on 02.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "public/prelude.h"

static result_t test(struct database_manager *database_manager) {
  uint64_t record_num = 1000;
  TRY(create_common_table(database_manager));
  CATCH(error, THROW(error))

  TRY(populate_common_table(database_manager, record_num));
  CATCH(error, THROW(error))

  struct statement_result *result = NULL;
  {
    struct predicate *where = predicate_of(expr_of(
        column_expr(TABLE_NAME(), STATUS_COL(), COLUMN_TYPE_BOOL),
        literal_expr((bool)true), comparison_operator(NEQ, COLUMN_TYPE_BOOL)));
    struct i_statement *delete = delete_statement_of(TABLE_NAME(), where);
    TRY(database_manager_execute_statement(database_manager, delete, &result));
    CATCH(error, THROW(error))
  }

  {
    struct predicate *where = predicate_of(LITERAL_TRUE);
    struct i_statement *query = query_statement_of(TABLE_NAME(), where, 0);
    TRY(database_manager_execute_statement(database_manager, query, &result));
    CATCH(error, THROW(error))

    struct record_view *view = statement_result_records(result);
    uint64_t remaining_records_num = 0;

    struct record *record = NULL;
    while (record_view_has_next(view)) {
      TRY(record_view_next(view, &record));
      CATCH(error, THROW(error))

      remaining_records_num++;

      TRY(record_print(record));
      CATCH(error, THROW(error))
    }

    if (remaining_records_num != (record_num - 1) / 3 + 1)
      THROW(TEST_ERROR("Records num mismatch"));
  }
  OK;
}

int main() { WRAP_WITH_DB("database", test); }