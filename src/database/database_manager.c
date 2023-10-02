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

  self->current_result = NULL;
  self->current_statement = NULL;
  self->was_statement_successful = true;
  OK;
}

result_t
database_manager_execute_statement(struct database_manager *self,
                                   struct i_statement *statement,
                                   struct statement_result **statement_result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  ASSERT_NOT_NULL(statement, ERROR_SOURCE);
  if (self->current_statement != NULL) {
    database_manager_finish_statement(self);
  }

  self->current_statement = statement;
  self->current_result = statement_result_new();
  *statement_result = self->current_result;
  TRY(statement->execute_impl(statement, self->table_manager,
                              self->current_result));
  CATCH(error, {
    self->was_statement_successful = false;
    THROW(error);
  })
  self->was_statement_successful = true;
  OK;
}

void database_manager_finish_statement(struct database_manager *self) {
  if (self->current_statement != NULL) {
    statement_destroy(self->current_statement);
    if (self->was_statement_successful) {
      statement_result_destroy(self->current_result);
    } else {
      free(self->current_result);
    }
    self->current_result = NULL;
    self->current_statement = NULL;
  }
}

void database_manager_destroy(struct database_manager *self) {
  database_manager_finish_statement(self);
  table_manager_destroy(self->table_manager);
  free(self);
}