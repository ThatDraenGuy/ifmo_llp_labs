//
// Created by draen on 02.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "database/prelude.h"

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
    uint64_t actual_record_num = 0;
    struct record *record = NULL;
    while (record_view_has_next(view)) {
      TRY(record_view_next(view, &record));
      CATCH(error, THROW(error))

      actual_record_num++;
      TRY(record_print(record));
      CATCH(error, THROW(error))
    }

    if (record_num != actual_record_num)
      THROW(TEST_ERROR("Records num mismatch"));
  }

  OK;
}

int main() { WRAP_WITH_DB("database", test); }