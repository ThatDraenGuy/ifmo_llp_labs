//
// Created by draen on 19.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TYPES_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TYPES_H

#include <stdint.h>

typedef enum {
  COLUMN_TYPE_INT32,
  COLUMN_TYPE_UINT64,
  COLUMN_TYPE_FLOAT,
  COLUMN_TYPE_STRING,
  COLUMN_TYPE_BOOL,
} column_type_t;

typedef union {
  int32_t int32_value;
  uint64_t uint64_value;
  float float_value;
  char *string_value;
  bool bool_value;
} column_value_t;

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TYPES_H
