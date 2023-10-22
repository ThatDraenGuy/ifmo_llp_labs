//
// Created by draen on 03.10.23.
//
#include "database/private/database/domain/expression/expression_interface.h"
#include "common/public/error/errors_common.h"

#define ERROR_SOURCE "EXPRESSION"

column_type_t expression_get_type(struct i_expression *self) {
  return self->type;
}

result_t expression_get(struct i_expression *self, struct record *record,
                        column_value_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  return self->get_impl(self, record, result);
}

struct i_expression *expression_clone(struct i_expression *self) {
  if (self == NULL)
    return NULL;
  return self->clone_impl(self);
}

void expression_destroy(struct i_expression *self) { self->destroy_impl(self); }