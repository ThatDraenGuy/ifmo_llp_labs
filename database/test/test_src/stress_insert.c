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

  struct statement_result *result = NULL;
  struct table_schema *schema = create_common_schema();
  {
    for (uint64_t iter_index = 0; iter_index < batch_num; iter_index++) {
      struct record_group *values = record_group_new();
      record_group_ctor(values, 1, schema);
      for (uint64_t index = 0; index < batch_size; index++) {
        uint64_t id = batch_size * iter_index + index;
        record_group_insert(values, 3, INSERT_COL_VAL(id),
                            INSERT_COL_VAL((bool)id % 3 == 0),
                            INSERT_COL_VAL((float)id / 3.f));
      }

      struct i_statement *insert = insert_statement_of(TABLE_NAME, values);
      TRY(database_manager_execute_statement(database_manager, insert,
                                             &result));
      CATCH(error, {
        table_schema_destroy(schema);
        THROW(error);
      })
      database_manager_finish_statement(database_manager);
    }
  }
  table_schema_destroy(schema);
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
