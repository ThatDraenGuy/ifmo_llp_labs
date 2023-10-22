//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSIONS_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSIONS_H

#include "database/public/database/domain/expression/expression_operator_interface.h"
#include "database/public/database/domain/expression/expressions.h"
#include "expression_interface.h"

struct literal_expression {
  struct i_expression parent;
  column_value_t value;
};

struct column_expression {
  struct i_expression parent;
  str_t table_name;
  str_t column_name;
};

struct operator_expression {
  struct i_expression parent;
  struct i_expression *first;
  struct i_expression *second;
  struct i_expression_operator *expression_operator;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSIONS_H
