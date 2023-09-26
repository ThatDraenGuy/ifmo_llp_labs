//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H

#include "record.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum { EQ, NEQ, LESS, LEQ, GREATER, GEQ } comparison_operator_t;

struct predicate_value;

struct predicate_value *literal_uint64(uint64_t value);
struct predicate_value *literal_int32(int32_t value);
struct predicate_value *literal_string(char *value);
struct predicate_value *literal_bool(bool value);
struct predicate_value *literal_float(float value);

#define literal(Value)                                                         \
  _Generic(Value,                                                              \
      uint64_t: literal_uint64,                                                \
      int32_t: literal_int32,                                                  \
      char *: literal_string,                                                  \
      bool: literal_bool,                                                      \
      float: literal_float)(Value)

struct predicate_value *column_value(char *column_name,
                                     column_type_t column_type);

struct predicate;

struct predicate *predicate_of(struct predicate_value *first,
                               struct predicate_value *second,
                               comparison_operator_t comparison_operator);

struct predicate *predicate_and(struct predicate *first,
                                struct predicate *second);

struct predicate *predicate_or(struct predicate *first,
                               struct predicate *second);

struct predicate *predicate_not(struct predicate *predicate);

result_t predicate_apply(struct predicate *self, struct record *record,
                         bool *result);
struct predicate *predicate_clone(struct predicate *self);

void predicate_destroy(struct predicate *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
