//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H

#include "database/public/database/domain/record/record.h"

struct i_expression;

column_type_t expression_get_expr_type(struct i_expression *self);
result_t expression_get(struct i_expression *self, struct record *record,
                        column_value_t *result);
struct i_expression *expression_clone(struct i_expression *self);
void expression_destroy(struct i_expression *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H
