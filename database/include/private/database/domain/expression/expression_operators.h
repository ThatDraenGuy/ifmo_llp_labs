//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H

#include "expression_operator_interface.h"
#include "public/database/domain/expression/expression_operators.h"

struct arithmetic_operator {
  struct i_expression_operator parent;
  arithmetic_operator_t arithmetic_operator;
};

struct comparison_operator {
  struct i_expression_operator parent;
  comparison_operator_t comparison_operator;
};

struct logical_operator {
  struct i_expression_operator parent;
  logical_operator_t logical_operator;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H
