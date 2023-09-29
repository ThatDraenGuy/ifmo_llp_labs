//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H

#include "schema.h"

struct record;

result_t record_get_value(struct record *self, str_t table_name,
                          str_t column_name, column_value_t *result);
result_t record_get_int32(struct record *self, str_t table_name,
                          str_t column_name, int32_t *result);
result_t record_get_uint64(struct record *self, str_t table_name,
                           str_t column_name, uint64_t *result);
result_t record_get_float(struct record *self, str_t table_name,
                          str_t column_name, float *result);
result_t record_get_string(struct record *self, str_t table_name,
                           str_t column_name, str_t *result);
result_t record_get_bool(struct record *self, str_t table_name,
                         str_t column_name, bool *result);

#define record_get(Record, TableName, ColumnName, Result)                      \
  _Generic(Result,                                                             \
      int32_t *: record_get_int32,                                             \
      uint64_t *: record_get_uint64,                                           \
      float *: record_get_float,                                               \
      str_t *: record_get_string,                                              \
      bool *: record_get_bool)(Record, TableName, ColumnName, Result)

void record_clear(struct record *self, size_t first_column, size_t last_column);
void record_clear_all(struct record *self);

struct record_update;

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_H
