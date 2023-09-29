//
// Created by draen on 25.09.23.
//

#include "private/database/domain/statement_interface.h"
#include "public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE "STATEMENT"

struct statement_result *statement_result_new() {
  return malloc(sizeof(struct statement_result));
}

struct record_view *statement_result_records(struct statement_result *self) {
  return self->records;
}

void statement_result_clear(struct statement_result *self) {
  switch (self->type) {
  case STATEMENT_RESULT_RECORDS:
    record_view_destroy(self->records);
    table_destroy(self->table);
    free(self->join_predicates);

    for (size_t index = 0; index < self->joins_num; index++) {
      table_destroy(self->join_tables[index]);
    }
    free(self->join_tables);

    break;
  case STATEMENT_RESULT_NONE:
    break;
  }
}
void statement_result_destroy(struct statement_result *self) {
  statement_result_clear(self);
  free(self);
}

result_t statement_execute(struct i_statement *self,
                           struct table_manager *table_manager,
                           struct statement_result *statement_result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  return self->execute_impl(self, table_manager, statement_result);
}

void statement_destroy(struct i_statement *self) { self->destroy_impl(self); }