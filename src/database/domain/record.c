//
// Created by draen on 27.09.23.
//

#include "private/database/domain/record.h"
#include "public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE "RECORD"
#define ERROR_TYPE "RECORD_ERROR"
enum error_code { NON_MATCHING_TYPE, COLUMN_NOT_FOUND };

const char *const error_messages[] = {
    [NON_MATCHING_TYPE] = "", [COLUMN_NOT_FOUND] = ""}; // TODO

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static result_t find_schema(struct column_schema_group *group,
                            str_t column_name, size_t *result_index) {
  for (size_t index = 0; index < group->columns_amount; index++) {
    struct column_schema *schema = group->schemas[index];
    if (str_eq(column_name, column_schema_get_name(schema))) {
      *result_index = index;
      OK;
    }
  }
  THROW(error_self(COLUMN_NOT_FOUND));
}

result_t record_get_value(struct record *self, str_t column_name,
                          column_value_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  size_t column_index = 0;
  TRY(find_schema(self->column_schema_group, column_name, &column_index));
  CATCH(error, THROW(error))
  *result = self->values[column_index];
  OK;
}

#define RECORD_GET_IMPL(Type, TypeName, ColumnTypeValue, ValueMapFunc)         \
  result_t record_get_##TypeName(struct record *self, str_t column_name,       \
                                 Type *result) {                               \
    ASSERT_NOT_NULL(self, ERROR_SOURCE);                                       \
    size_t column_index = 0;                                                   \
    TRY(find_schema(self->column_schema_group, column_name, &column_index));   \
    CATCH(error, THROW(error))                                                 \
    struct column_schema *schema =                                             \
        self->column_schema_group->schemas[column_index];                      \
    if (column_schema_get_type(schema) != ColumnTypeValue)                     \
      THROW(error_self(NON_MATCHING_TYPE));                                    \
                                                                               \
    *result = ValueMapFunc(self->values[column_index].TypeName##_value);       \
    OK;                                                                        \
  }

RECORD_GET_IMPL(uint64_t, uint64, COLUMN_TYPE_UINT64, )
RECORD_GET_IMPL(int32_t, int32, COLUMN_TYPE_INT32, )
RECORD_GET_IMPL(float, float, COLUMN_TYPE_FLOAT, )
RECORD_GET_IMPL(bool, bool, COLUMN_TYPE_BOOL, )
RECORD_GET_IMPL(str_t, string, COLUMN_TYPE_STRING, string_as_str)

void record_clear(struct record *self) {
  for (size_t index = 0; index < self->column_schema_group->columns_amount;
       index++) {
    column_value_t value = self->values[index];
    struct column_schema *column_schema =
        self->column_schema_group->schemas[index];

    if (column_schema_get_type(column_schema) == COLUMN_TYPE_STRING &&
        value.string_value != NULL)
      string_destroy(value.string_value);
  }
}

struct record *record_new(struct column_schema_group *column_schema_group) {
  struct record *self =
      malloc(sizeof(struct record) +
             sizeof(column_value_t) * column_schema_group->columns_amount);
  self->column_schema_group = column_schema_group;

  for (size_t index = 0; index < self->column_schema_group->columns_amount;
       index++) {
    self->values[index] = (column_value_t){0};
  }
  return self;
}

void record_destroy(struct record *self) {
  record_clear(self);
  free(self);
}