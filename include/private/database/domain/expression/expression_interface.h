//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H

#include "public/database/domain/expression/expression_interface.h"
#include "public/database/domain/record/record.h"

struct i_expression {
  column_type_t type;
  result_t (*get_impl)(struct i_expression *self, struct record *record,
                       column_value_t *result);
  struct i_expression *(*clone_impl)(struct i_expression *self);
  void (*destroy_impl)(struct i_expression *self);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_EXPRESSION_INTERFACE_H
