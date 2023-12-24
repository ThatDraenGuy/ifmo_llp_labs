//
// Created by draen on 02.10.23.
//
#include "common_table_operations.h"

struct table_schema *create_common_schema() {
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, TABLE_NAME, 3);
  table_schema_add_column(schema, ID_COL, COLUMN_TYPE_UINT64);
  table_schema_add_column(schema, STATUS_COL, COLUMN_TYPE_BOOL);
  table_schema_add_column(schema, NUM_COL, COLUMN_TYPE_FLOAT);
  return schema;
}

result_t create_common_table(struct database_manager *database_manager) {
  struct statement_result *result = NULL;
  struct table_schema *schema = create_common_schema();
  {
    struct i_statement *create_table = create_table_statement_of(schema);
    TRY(database_manager_execute_statement(database_manager, create_table,
                                           &result));
    CATCH(error, {
      table_schema_destroy(schema);
      THROW(error);
    })
  }
  database_manager_finish_statement(database_manager);
  table_schema_destroy(schema);
  OK;
}

result_t populate_common_table(struct database_manager *database_manager,
                               uint64_t record_num) {

  struct statement_result *result = NULL;
  struct table_schema *schema = create_common_schema();
  {
    struct record_group *values = record_group_new();
    record_group_ctor(values, 1, schema);

    for (uint64_t id = 0; id < record_num; id++) {
      bool status = id % 3 == 0;
      float num = (float)id / 3.f;
      record_group_insert(values, 3, INSERT_COL_VAL(id),
                          INSERT_COL_VAL((bool)status), INSERT_COL_VAL(num));
    }

    struct i_statement *insert = insert_statement_of(TABLE_NAME, values);
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