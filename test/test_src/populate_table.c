//
// Created by draen on 02.10.23.
//

#include "create_common_table.h"
#include "database_manager_wrapper.h"
#include "public/prelude.h"

uint64_t record_num = 1000;

static result_t test(struct database_manager *database_manager) {
  TRY(create_common_table(database_manager));
  CATCH(error, THROW(error))

  struct statement_result *result = NULL;
  struct table_schema *schema = create_common_schema();
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema);

    for (uint64_t id = 0; id < record_num; id++) {
      bool status = id % 3 == 0;
      float num = (float)id / 3.f;
      record_group_insert(values, 3, COLUMN_VALUE(id), COLUMN_VALUE(status),
                          COLUMN_VALUE(num));
    }

    struct i_statement *insert = insert_statement_of(TABLE_NAME(), values);
    TRY(database_manager_execute_statement(database_manager, insert, &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
  }
  database_manager_finish_statement(database_manager);
  table_schema_destroy(schema);
  OK;
}

int main() { WRAP_WITH_DB("database", test); }