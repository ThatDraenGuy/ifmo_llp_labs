//
// Created by draen on 18.09.23.
//

#include "private/database/domain/record.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <string.h>

static const char *const error_source = "RECORD";
static const char *const error_type = "RECORD_ERROR";

enum error_code {
  RECORD_SIZE_EXCEEDED,
  INCORRECT_COLUMN_TYPE,
  UNKNOWN_COLUMN_NAME,
  INDEX_OUT_OF_BOUNDS
};

static const char *const error_messages[] = {
    [RECORD_SIZE_EXCEEDED] = "Record size was exceeded!",
    [INCORRECT_COLUMN_TYPE] = "Incorrect column type!",
    [UNKNOWN_COLUMN_NAME] = "Unknown column name!",
    [INDEX_OUT_OF_BOUNDS] = "Index out of bounds"};

static struct error *error_self(enum error_code error_code) {
  return error_new(error_source, error_type, (error_code_t){error_code},
                   error_messages[error_code]);
}

struct record *record_new() { return malloc(sizeof(struct record)); }

void record_ctor(struct record *self, size_t column_amount) {
  self->column_amount = column_amount;
  self->current_entry_index = 0;
  self->entries = malloc(sizeof(struct record_entry) * column_amount);
}

void record_clear(struct record *self) { self->current_entry_index = 0; }

result_t record_insert_value(struct record *self, char *column_name,
                             column_value_t value, column_type_t type) {
  ASSERT_NOT_NULL(self, error_source);
  if (self->current_entry_index >= self->column_amount) {
    THROW(error_self(RECORD_SIZE_EXCEEDED));
  }

  self->entries[self->current_entry_index].schema.name =
      column_name; // TODO think about strings
  self->entries[self->current_entry_index].schema.type = type;
  self->entries[self->current_entry_index].value = value;

  self->current_entry_index++;

  OK;
}

result_t record_insert_int32(struct record *self, char *column_name,
                             int32_t value) {
  return record_insert_value(self, column_name,
                             (column_value_t){.int32_value = value},
                             COLUMN_TYPE_INT32);
}
result_t record_insert_uint64(struct record *self, char *column_name,
                              uint64_t value) {
  return record_insert_value(self, column_name,
                             (column_value_t){.uint64_value = value},
                             COLUMN_TYPE_UINT64);
}

result_t record_insert_float(struct record *self, char *column_name,
                             float value) {
  return record_insert_value(self, column_name,
                             (column_value_t){.float_value = value},
                             COLUMN_TYPE_FLOAT);
}
result_t record_insert_string(struct record *self, char *column_name,
                              char *value) { // TODO string copy
  return record_insert_value(self, column_name,
                             (column_value_t){.string_value = value},
                             COLUMN_TYPE_STRING);
}
result_t record_insert_bool(struct record *self, char *column_name,
                            bool value) {
  return record_insert_value(self, column_name,
                             (column_value_t){.bool_value = value},
                             COLUMN_TYPE_BOOL);
}

static result_t get_entry_at(struct record *self, size_t column_index,
                             struct record_entry **result) {
  ASSERT_NOT_NULL(self, error_source);
  if (column_index >= self->column_amount)
    THROW(error_self(INDEX_OUT_OF_BOUNDS));

  *result = &self->entries[column_index];
  OK;
}

result_t record_copy_into(struct record *self, struct record *target) {
  for (size_t index = 0; index < self->current_entry_index; index++) {
    struct record_entry *entry = NULL;
    TRY(get_entry_at(self, index, &entry));
    CATCH(error, THROW(error))

    TRY(record_insert_value(target, entry->schema.name, entry->value,
                            entry->schema.type));
    CATCH(error, THROW(error))
  }
  OK;
}

result_t record_get_value_at(struct record *self, size_t column_index,
                             column_value_t *result_value,
                             column_type_t *result_type) {
  struct record_entry *entry = NULL;
  TRY(get_entry_at(self, column_index, &entry));
  CATCH(error, THROW(error))

  *result_value = entry->value;
  *result_type = entry->schema.type;
  OK;
}

#define RECORD_GET_AT_IMPL(Type, TypeName, ColumnTypeValue)                    \
  result_t record_get_##TypeName##_at(struct record *self,                     \
                                      size_t column_index, Type *result) {     \
    ASSERT_NOT_NULL(self, error_source);                                       \
    if (column_index >= self->column_amount)                                   \
      THROW(error_self(INDEX_OUT_OF_BOUNDS));                                  \
                                                                               \
    struct record_entry *entry = &self->entries[column_index];                 \
    if (entry->schema.type != ColumnTypeValue)                                 \
      THROW(error_self(INCORRECT_COLUMN_TYPE));                                \
    *result = entry->value.TypeName##_value;                                   \
    OK;                                                                        \
  }
RECORD_GET_AT_IMPL(int32_t, int32, COLUMN_TYPE_INT32);
RECORD_GET_AT_IMPL(uint64_t, uint64, COLUMN_TYPE_UINT64);
RECORD_GET_AT_IMPL(float, float, COLUMN_TYPE_FLOAT);
RECORD_GET_AT_IMPL(char *, string, COLUMN_TYPE_STRING);
RECORD_GET_AT_IMPL(bool, bool, COLUMN_TYPE_BOOL);

static struct record_entry *find_entry(struct record *self, char *column_name) {
  for (size_t index = 0; index < self->current_entry_index; index++) {
    struct record_entry *entry = &self->entries[index];
    if (strcmp(entry->schema.name, column_name) == 0) {
      return entry;
    }
  }

  return NULL;
}

result_t record_get_value(struct record *self, char *column_name,
                          column_value_t *result) {
  ASSERT_NOT_NULL(self, error_source);

  struct record_entry *entry = find_entry(self, column_name);
  if (entry == NULL)
    THROW(error_self(UNKNOWN_COLUMN_NAME));

  *result = entry->value;
  OK;
}

#define RECORD_GET_IMPL(Type, TypeName, ColumnTypeValue)                       \
  result_t record_get_##TypeName(struct record *self, char *column_name,       \
                                 Type *result) {                               \
    ASSERT_NOT_NULL(self, error_source);                                       \
                                                                               \
    struct record_entry *entry = find_entry(self, column_name);                \
    if (entry == NULL)                                                         \
      THROW(error_self(UNKNOWN_COLUMN_NAME));                                  \
                                                                               \
    if (entry->schema.type != ColumnTypeValue)                                 \
      THROW(error_self(INCORRECT_COLUMN_TYPE));                                \
    *result = entry->value.TypeName##_value;                                   \
    OK;                                                                        \
  }
RECORD_GET_IMPL(int32_t, int32, COLUMN_TYPE_INT32);
RECORD_GET_IMPL(uint64_t, uint64, COLUMN_TYPE_UINT64);
RECORD_GET_IMPL(float, float, COLUMN_TYPE_FLOAT);
RECORD_GET_IMPL(char *, string, COLUMN_TYPE_STRING);
RECORD_GET_IMPL(bool, bool, COLUMN_TYPE_BOOL);

void record_destroy(struct record *self) {
  free(self->entries);
  free(self);
}