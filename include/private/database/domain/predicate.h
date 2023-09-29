//
// Created by draen on 19.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_PREDICATE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_PREDICATE_H

#include "public/database/domain/predicate.h"

struct predicate_value {
  column_type_t type;
  result_t (*get_value_impl)(struct predicate_value *self,
                             struct record *record, column_value_t *result);
  struct predicate_value *(*clone_impl)(struct predicate_value *self);
  void (*destroy_impl)(struct predicate_value *self);
};

struct literal_predicate_value {
  struct predicate_value parent;
  column_value_t value;
};

struct column_predicate_value {
  struct predicate_value parent;
  str_t table_name;
  str_t column_name;
};

struct predicate {
  result_t (*apply_impl)(struct predicate *self, struct record *record,
                         bool *result);
  struct predicate *(*clone_impl)(struct predicate *self);
  void (*destroy_impl)(struct predicate *self);
};

struct predicate_of_impl {
  struct predicate parent;
  struct predicate_value *first;
  struct predicate_value *second;
  comparison_operator_t comparison_operator;
};

typedef enum { AND, OR, NOT } logical_operator_t;

struct predicate_logical_impl {
  struct predicate parent;
  struct predicate *first;
  struct predicate *second;
  logical_operator_t logical_operator;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_PREDICATE_H
