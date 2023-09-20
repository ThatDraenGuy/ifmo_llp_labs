//
// Created by draen on 18.09.23.
//
#include "public/database/record_serialization.h"
#include "private/database/domain/record.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

static const char *const error_source = "RECORD_SERIALIZATION";
static const char *const error_type = "RECORD_SERIALIZATION_ERROR";
enum error_code { INCORRECT_RECORD_SIZE, INCORRECT_ITEM_SIZE, INVALID_DATA };

static const char *const error_messages[] = {
    [INCORRECT_RECORD_SIZE] = "Record is too small!",
    [INCORRECT_ITEM_SIZE] = "Item is too small!",
    [INVALID_DATA] = "Item data is invalid!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(error_source, error_type, (error_code_t){error_code},
                   error_messages[error_code]);
}

static bool verify_size(item_t item, const size_t *item_offset,
                        size_t column_data_size) {
  return item.size - *item_offset >= column_data_size;
}

#define READ_IMPL(Type, TypeName)                                              \
  static result_t read_##TypeName(item_t item, size_t *item_offset,            \
                                  Type *result) {                              \
    if (!verify_size(item, item_offset, sizeof(Type)))                         \
      return result_err(error_self(INCORRECT_ITEM_SIZE));                      \
                                                                               \
    *result = *(Type *)(item.data + *item_offset);                             \
    *item_offset += sizeof(Type);                                              \
    return OK;                                                                 \
  }
READ_IMPL(int32_t, int32);
READ_IMPL(uint64_t, uint64);
READ_IMPL(float, float);

static result_t read_string(item_t item, size_t *item_offset, char **result) {
  uint64_t string_size = 0;
  TRY(read_uint64(item, item_offset, &string_size));
  CATCH(error, PROPAGATE)

  if (!verify_size(item, item_offset, string_size))
    return result_err(error_self(INCORRECT_ITEM_SIZE));

  *result = malloc(sizeof(char) * string_size);

  strcpy(*result, (char *)(item.data + *item_offset));
  *item_offset += string_size;
  return OK;
}

static result_t read_bool(item_t item, size_t *item_offset, bool *result) {
  size_t bool_item_size = sizeof(uint8_t);
  if (!verify_size(item, item_offset, bool_item_size))
    return result_err(error_self(INCORRECT_ITEM_SIZE));

  uint8_t value = *(uint8_t *)(item.data + *item_offset);
  if (value != false && value != true)
    return result_err(error_self(INVALID_DATA));
  *result = (bool)value;
  *item_offset += bool_item_size;
  return OK;
}

static result_t deserialize_column(item_t item, size_t *item_offset,
                                   struct column_schema *column_schema,
                                   struct record *target) {
  switch (column_schema->type) {
  case COLUMN_TYPE_UINT64: {
    uint64_t value = 0;
    TRY(read_uint64(item, item_offset, &value));
    CATCH(error, PROPAGATE)

    TRY(record_insert(target, column_schema->name, value));
    CATCH(error, PROPAGATE)
    break;
  }
  case COLUMN_TYPE_INT32: {
    int32_t value = 0;
    TRY(read_int32(item, item_offset, &value));
    CATCH(error, PROPAGATE)

    TRY(record_insert(target, column_schema->name, value));
    CATCH(error, PROPAGATE)
    break;
  }
  case COLUMN_TYPE_FLOAT: {
    float value = 0;
    TRY(read_float(item, item_offset, &value));
    CATCH(error, PROPAGATE)

    TRY(record_insert(target, column_schema->name, value));
    CATCH(error, PROPAGATE)
    break;
  }
  case COLUMN_TYPE_STRING: {
    char *value = NULL;
    TRY(read_string(item, item_offset, &value));
    CATCH(error, PROPAGATE)

    TRY(record_insert(target, column_schema->name, value));
    CATCH(error, {
      free(value);
      PROPAGATE;
    })
    break;
  }
  case COLUMN_TYPE_BOOL: {
    bool value = false;
    TRY(read_bool(item, item_offset, &value));
    CATCH(error, PROPAGATE)

    TRY(record_insert(target, column_schema->name, value));
    CATCH(error, PROPAGATE)
    break;
  }
  }

  return OK;
}

result_t record_deserialize(item_t item, struct table_schema *schema,
                            struct record *target) {
  ASSERT_NOT_NULL(target, error_source);

  if (target->column_amount - target->current_entry_index <
      table_schema_get_column_amount(schema))
    return result_err(error_self(INCORRECT_RECORD_SIZE));

  size_t current_item_offset = 0;
  struct column_schema_iterator *it = table_schema_get_columns(schema);
  while (column_schema_iterator_has_next(it)) {
    struct column_schema *column_schema = NULL;
    TRY(column_schema_iterator_next(it, &column_schema));
    CATCH(error, {
      column_schema_iterator_destroy(it);
      PROPAGATE;
    })

    TRY(deserialize_column(item, &current_item_offset, column_schema, target));
    CATCH(error, {
      column_schema_iterator_destroy(it);
      PROPAGATE;
    })
  }
  column_schema_iterator_destroy(it);

  return OK;
}

static void calculate_column_size(column_value_t column_value,
                                  column_type_t column_type,
                                  size_t *item_size) {
  switch (column_type) {

  case COLUMN_TYPE_INT32: {
    *item_size += sizeof(int32_t);
    break;
  }
  case COLUMN_TYPE_UINT64: {
    *item_size += sizeof(uint64_t);
    break;
  }
  case COLUMN_TYPE_FLOAT: {
    *item_size += sizeof(float);
    break;
  }
  case COLUMN_TYPE_STRING: {
    size_t string_size = strlen(column_value.string_value) + 1;
    *item_size += string_size + sizeof(uint64_t);
    break;
  }
  case COLUMN_TYPE_BOOL: {
    *item_size += sizeof(uint8_t);
    break;
  }
  }
}

static void serialize_column(item_t item, size_t *item_offset,
                             column_type_t column_type,
                             column_value_t column_value) {
  switch (column_type) {

  case COLUMN_TYPE_INT32: {
    *(int32_t *)(item.data + *item_offset) = column_value.int32_value;
    *item_offset += sizeof(int32_t);
    break;
  }
  case COLUMN_TYPE_UINT64: {
    *(uint64_t *)(item.data + *item_offset) = column_value.uint64_value;
    *item_offset += sizeof(uint64_t);
    break;
  }
  case COLUMN_TYPE_FLOAT: {
    *(float *)(item.data + *item_offset) = column_value.float_value;
    *item_offset += sizeof(float);
    break;
  }
  case COLUMN_TYPE_STRING: {
    size_t string_size = strlen(column_value.string_value) + 1;
    *(uint64_t *)(item.data + *item_offset) = (uint64_t)string_size;
    *item_offset += sizeof(uint64_t);

    strcpy((char *)(item.data + *item_offset), column_value.string_value);

    *item_offset += string_size;
    break;
  }
  case COLUMN_TYPE_BOOL: {
    *(uint8_t *)(item.data + *item_offset) = column_value.bool_value;
    *item_offset += sizeof(uint8_t);
    break;
  }
  }
}

item_t record_serialize(struct record *record) {
  size_t item_size = 0;
  for (size_t index = 0; index < record->current_entry_index; index++) {
    column_value_t column_value = {0};
    column_type_t column_type = {0};
    record_get_value_at(record, index, &column_value, &column_type);
    calculate_column_size(column_value, column_type, &item_size);
  }

  void *item_data = malloc(item_size);
  item_t item = (item_t){.size = item_size, .data = item_data};

  size_t item_offset = 0;
  for (size_t index = 0; index < record->current_entry_index; index++) {
    column_value_t column_value = {0};
    column_type_t column_type = {0};
    record_get_value_at(record, index, &column_value, &column_type);
    serialize_column(item, &item_offset, column_type, column_value);
  }

  return item;
}