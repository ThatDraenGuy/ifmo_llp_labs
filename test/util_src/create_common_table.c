//
// Created by draen on 02.10.23.
//
#include "create_common_table.h"

struct table_schema *create_common_schema() {
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, TABLE_NAME(), 3);
  table_schema_add_column(schema, ID_COL(), COLUMN_TYPE_UINT64);
  table_schema_add_column(schema, STATUS_COL(), COLUMN_TYPE_BOOL);
  table_schema_add_column(schema, NUM_COL(), COLUMN_TYPE_FLOAT);
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