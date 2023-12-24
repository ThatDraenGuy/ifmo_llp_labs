//
// Created by draen on 23.12.23.
//

#include "database/public/database/domain/statement/statements.h"
#include "transfer_server/private/mapper.h"

result_t map_table_schema(TableSchema *schema, struct table_schema **result) {}

result_t map_create_table(CreateTableStmt *stmt, struct i_statement **result) {
  TableSchema *g_schema;
  g_object_get(stmt, "schema", &g_schema, NULL);

  struct table_schema *schema;
  TRY(map_table_schema(g_schema, &schema));
  CATCH(error, THROW(error))
}

result_t map_stmt(const Statement *stmt, struct i_statement **result) {
  CreateTableStmt *create_table;
  DropTableStmt *drop_table;
  QueryStmt *select;
  InsertStmt *insert;
  UpdateStmt *update;
  DeleteStmt *delete;
  g_object_get(stmt, "createTableStmt", &create_table, "dropTableStmt",
               &drop_table, "queryStmt", &select, "insertStmt", &insert,
               "updateStmt", &update, "deleteStmt", &delete, NULL);
}