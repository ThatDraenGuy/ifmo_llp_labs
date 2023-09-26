//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H

#include "public/util/result.h"
#include "schema.h"
#include <stdbool.h>
#include <stdint.h>
struct record;

struct record *record_new();
void record_ctor(struct record *self);
void record_clear(struct record *self);

result_t record_get_value_at(struct record *self, size_t column_index,
                             column_value_t *result_value,
                             column_type_t *result_type);

result_t record_get_int32_at(struct record *self, size_t column_index,
                             int32_t *result);
result_t record_get_uint64_at(struct record *self, size_t column_index,
                              uint64_t *result);
result_t record_get_float_at(struct record *self, size_t column_index,
                             float *result);
result_t record_get_string_at(struct record *self, size_t column_index,
                              char **result);
result_t record_get_bool_at(struct record *self, size_t column_index,
                            bool *result);

#define record_get_at(Record, Index, Result)                                   \
  _Generic(Result,                                                             \
      int32_t *: record_get_int32_at,                                          \
      uint64_t *: record_get_uint64_at,                                        \
      float *: record_get_float_at,                                            \
      char **: record_get_string_at,                                           \
      bool *: record_get_bool_at)(Record, Index, Result)

result_t record_get_value(struct record *self, char *column_name,
                          column_value_t *result);

result_t record_get_int32(struct record *self, char *column_name,
                          int32_t *result);
result_t record_get_uint64(struct record *self, char *column_name,
                           uint64_t *result);
result_t record_get_float(struct record *self, char *column_name,
                          float *result);
result_t record_get_string(struct record *self, char *column_name,
                           char **result);
result_t record_get_bool(struct record *self, char *column_name, bool *result);

#define record_get(Record, ColumnName, Result)                                 \
  _Generic(Result,                                                             \
      int32_t *: record_get_int32,                                             \
      uint64_t *: record_get_uint64,                                           \
      float *: record_get_float,                                               \
      char **: record_get_string,                                              \
      bool *: record_get_bool)(Record, ColumnName, Result)

result_t record_insert_int32(struct record *self, char *column_name,
                             int32_t value);
result_t record_insert_uint64(struct record *self, char *column_name,
                              uint64_t value);
result_t record_insert_float(struct record *self, char *column_name,
                             float value);
result_t record_insert_string(struct record *self, char *column_name,
                              char *value);
result_t record_insert_bool(struct record *self, char *column_name, bool value);

#define record_insert(Record, ColumnName, Value)                               \
  _Generic(Value,                                                              \
      int32_t: record_insert_int32,                                            \
      uint64_t: record_insert_uint64,                                          \
      float: record_insert_float,                                              \
      char *: record_insert_string,                                            \
      bool: record_insert_bool)(Record, ColumnName, Value)

void record_destroy(struct record *self);

struct record_update;

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H
