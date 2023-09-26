//
// Created by draen on 23.09.23.
//

#include "private/database/database_manager.h"
#include "private/database/domain/statement_interface.h"
#include "public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE "DATABASE_MANAGER"

struct database_manager *database_manager_new() {
  return malloc(sizeof(struct database_manager));
}

result_t database_manager_ctor(struct database_manager *self, char *file_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  self->table_manager = table_manager_new();
  TRY(table_manager_ctor(self->table_manager, file_name));
  CATCH(error, THROW(error))

  OK;
}

result_t
database_manager_execute_statement(struct database_manager *self,
                                   struct i_statement *statement,
                                   struct statement_result *statement_result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  ASSERT_NOT_NULL(statement, ERROR_SOURCE);
  return statement->execute_impl(statement, self->table_manager,
                                 statement_result);
}

void database_manager_destroy(struct database_manager *self) {
  table_manager_destroy(self->table_manager);
  free(self);
}