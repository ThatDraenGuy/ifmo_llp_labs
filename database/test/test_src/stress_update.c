//
// Created by draen on 03.10.23.
//
//
// Created by draen on 03.10.23.
//

#include "common_table_operations.h"
#include "common_test_utils.h"
#include "public/prelude.h"
#include <stdio.h>
#include <stdlib.h>

uint64_t batch_size = 1000; // to not put too much records in memory
uint64_t batch_num = 10;
static result_t test(struct database_manager *database_manager) {
  struct statement_result *result = NULL;
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