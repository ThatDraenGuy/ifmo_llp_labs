//
// Created by draen on 03.10.23.
//
//
// Created by draen on 03.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "database/prelude.h"
#include <stdio.h>
#include <stdlib.h>

uint64_t batch_size = 1000; // to not put too much records in memory
uint64_t batch_num = 10;
static result_t test(struct database_manager *database_manager) {
  TRY(create_common_table(database_manager));
  CATCH(error, error_destroy(error))
  TRY(populate_common_table(database_manager, 10));
  CATCH(error, error_destroy(error))

  struct statement_result *result = NULL;
  {
    struct predicate *where = predicate_of(expr_of(
        column_expr(TABLE_NAME(), STATUS_COL(), COLUMN_TYPE_BOOL),
        literal_expr((bool)true), comparison_operator(EQ, COLUMN_TYPE_BOOL)));
    struct i_statement *query = query_statement_of(TABLE_NAME(), where, 0);
    TRY(database_manager_execute_statement(database_manager, query, &result));
    CATCH(error, THROW(error))

    struct record_view *view = statement_result_records(result);

    struct record *record = NULL;
    while (record_view_has_next(view)) {
      TRY(record_view_next(view, &record));
      CATCH(error, THROW(error))
    }
  }
  OK;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    char *input = argv[1];
    batch_num = strtoul(input, NULL, 10);
  } else {
    batch_num = 10;
  }
  printf("%lu\n", batch_num);
  WRAP_WITH_DB("database", test);
}