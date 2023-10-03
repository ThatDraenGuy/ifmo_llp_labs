//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H

#include "public/database/domain/expression/expression_operator_interface.h"

struct i_expression_operator {
  column_type_t first_type;
  column_type_t second_type;
  column_type_t result_type;
  result_t (*apply_impl)(struct i_expression_operator *self,
                         column_value_t first, column_value_t second,
                         column_value_t *result);
  void (*destroy_impl)(struct i_expression_operator *self);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATOR_INTERFACE_H
