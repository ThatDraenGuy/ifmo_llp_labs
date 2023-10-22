//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H

#include "database/public/database/domain/types.h"

struct i_expression_operator;

column_type_t
expression_operator_get_first_operand_type(struct i_expression_operator *self);
column_type_t
expression_operator_get_second_operand_type(struct i_expression_operator *self);
column_type_t
expression_operator_get_result_operand_type(struct i_expression_operator *self);

result_t expression_operator_apply(struct i_expression_operator *self,
                                   column_value_t first, column_value_t second,
                                   column_value_t *result);
struct i_expression_operator *
expression_operator_clone(struct i_expression_operator *self);
void expression_operator_destroy(struct i_expression_operator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H
