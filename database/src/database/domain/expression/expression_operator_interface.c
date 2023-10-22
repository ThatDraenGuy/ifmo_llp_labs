//
// Created by draen on 03.10.23.
//
#include "database/private/database/domain/expression/expression_operator_interface.h"
#include "common/public/error/errors_common.h"

#define ERROR_SOURCE "EXPRESSION_OPERATOR"

column_type_t
expression_operator_get_first_operand_type(struct i_expression_operator *self) {
  return self->first_type;
}

column_type_t expression_operator_get_second_operand_type(
    struct i_expression_operator *self) {
  return self->second_type;
}

column_type_t expression_operator_get_result_operand_type(
    struct i_expression_operator *self) {
  return self->result_type;
}

result_t expression_operator_apply(struct i_expression_operator *self,
                                   column_value_t first, column_value_t second,
                                   column_value_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  return self->apply_impl(self, first, second, result);
}

struct i_expression_operator *
expression_operator_clone(struct i_expression_operator *self) {
  if (self == NULL)
    return NULL;
  return self->clone_impl(self);
}

void expression_operator_destroy(struct i_expression_operator *self) {
  return self->destroy_impl(self);
}