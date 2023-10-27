//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSIONS_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSIONS_H

#include "common/public/domain/column_types.h"
#include "common/public/util/string.h"
#include "expression_operator_interface.h"
#include <stdbool.h>
#include <stdint.h>

struct i_expression *literal_expr_uint64(uint64_t value);
struct i_expression *literal_expr_int32(int32_t value);
struct i_expression *literal_expr_string(str_t value);
struct i_expression *literal_expr_bool(bool value);
struct i_expression *literal_expr_float(float value);

#define literal_expr(Value)                                                    \
  _Generic(Value,                                                              \
      uint64_t: literal_expr_uint64,                                           \
      int32_t: literal_expr_int32,                                             \
      str_t: literal_expr_string,                                              \
      bool: literal_expr_bool,                                                 \
      float: literal_expr_float)(Value)

#define LITERAL_TRUE literal_expr_bool(true)

struct i_expression *column_expr(str_t table_name, str_t column_name,
                                 column_type_t column_type);

struct i_expression *expr_of(struct i_expression *first,
                             struct i_expression *second,
                             struct i_expression_operator *expression_operator);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSIONS_H
