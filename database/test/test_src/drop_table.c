//
// Created by draen on 02.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"

static result_t test(struct database_manager *database_manager) {
  TRY(create_common_table(database_manager));
  CATCH(error, THROW(error))

  struct statement_result *result = NULL;
  {
    struct i_statement *drop = drop_table_statement_of(TABLE_NAME);
    TRY(database_manager_execute_statement(database_manager, drop, &result));
    CATCH(error, THROW(error))
  }
  OK;
}

int main() { WRAP_WITH_DB("database", test); }