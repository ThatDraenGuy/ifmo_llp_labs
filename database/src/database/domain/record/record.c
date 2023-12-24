//
// Created by draen on 27.09.23.
//

#include "database/private/database/domain/record/record.h"
#include "common/public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE STR_OF("RECORD")
#define ERROR_TYPE STR_OF("RECORD_ERROR")
enum error_code { NON_MATCHING_TYPE, COLUMN_NOT_FOUND };

static const str_t error_messages[] = {
    [NON_MATCHING_TYPE] = STR_OF("Requested column type differs from expected"),
    [COLUMN_NOT_FOUND] = STR_OF("No such column")};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static void clear_record_value(struct record *self, size_t index) {
  column_value_t value = self->values[index];
  struct column_schema *column_schema =
      self->column_schema_group->schemas[index];

  if (column_schema_get_col_type(column_schema) == COLUMN_TYPE_STRING &&
      string_is_null(value.string_value)) {
    string_destroy(value.string_value);
    value.string_value = STRING_NULL;
  }
}

static result_t find_schema(struct column_schema_group *group, str_t table_name,
                            str_t column_name, size_t *result_index) {
  for (size_t index = 0; index < group->columns_amount; index++) {
    struct column_schema *schema = group->schemas[index];
    if (str_eq(table_name, column_schema_get_table_name(schema)) &&
        str_eq(column_name, column_schema_get_name(schema))) {
      *result_index = index;
      OK;
    }
  }
  THROW(error_self(COLUMN_NOT_FOUND));
}
result_t record_set_value(struct record *self, str_t table_name,
                          str_t column_name, column_value_t value) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  size_t column_index = 0;
  TRY(find_schema(self->column_schema_group, table_name, column_name,
                  &column_index));
  CATCH(error, THROW(error))

  clear_record_value(self, column_index);

  if (column_schema_get_col_type(
          self->column_schema_group->schemas[column_index]) ==
      COLUMN_TYPE_STRING) {
    self->values[column_index].string_value = string_clone(value.string_value);
  } else {
    self->values[column_index] = value;
  }

  OK;
}

result_t record_get_value(struct record *self, str_t table_name,
                          str_t column_name, column_value_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  size_t column_index = 0;
  TRY(find_schema(self->column_schema_group, table_name, column_name,
                  &column_index));
  CATCH(error, THROW(error))
  *result = self->values[column_index];
  OK;
}

#define RECORD_GET_IMPL(Type, TypeName, ColumnTypeValue, ValueMapFunc)         \
  result_t record_get_##TypeName(struct record *self, str_t table_name,        \
                                 str_t column_name, Type *result) {            \
    ASSERT_NOT_NULL(self, ERROR_SOURCE);                                       \
    size_t column_index = 0;                                                   \
    TRY(find_schema(self->column_schema_group, table_name, column_name,        \
                    &column_index));                                           \
    CATCH(error, THROW(error))                                                 \
    struct column_schema *schema =                                             \
        self->column_schema_group->schemas[column_index];                      \
    if (column_schema_get_col_type(schema) != ColumnTypeValue)                 \
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

void record_clear(struct record *self, size_t first_column,
                  size_t last_column) {
  for (size_t index = first_column; index < last_column; index++) {
    clear_record_value(self, index);
  }
}
void record_clear_all(struct record *self) {
  record_clear(self, 0, self->column_schema_group->columns_amount);
}

struct record *record_new(struct column_schema_group *column_schema_group) {
  struct record *self = calloc(1, sizeof(struct record) +
                                      sizeof(column_value_t) *
                                          column_schema_group->columns_amount);
  self->column_schema_group = column_schema_group;
  return self;
}

void record_destroy(struct record *self) {
  record_clear_all(self);
  free(self);
}