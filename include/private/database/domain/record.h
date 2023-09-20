//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H

#include "private/database/domain/schema.h"
#include "public/database/domain/record.h"
#include "public/storage/domain/page_group.h"
#include "schema.h"

struct record_entry {
  struct column_schema schema;
  column_value_t value;
};

struct record {
  size_t column_amount;
  size_t current_entry_index;
  struct record_entry *entries;
};

struct record *record_new();
void record_ctor(struct record *self, size_t column_amount);
result_t record_copy_into(struct record *self, struct record *target);
void record_clear(struct record *self);

result_t record_insert_value(struct record *self, char *column_name,
                             column_value_t value, column_type_t type);

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

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
