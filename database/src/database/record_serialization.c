//
// Created by draen on 18.09.23.
//
#include "database/public/database/record_serialization.h"
#include "common/public/error/errors_common.h"
#include "database/private/database/domain/record/record.h"
#include <malloc.h>

#define ERROR_SOURCE STR_OF("RECORD_SERIALIZATION")
#define ERROR_TYPE STR_OF("RECORD_SERIALIZATION_ERROR")
enum error_code { INCORRECT_ITEM_SIZE, INVALID_DATA };

static const str_t error_messages[] = {
    [INCORRECT_ITEM_SIZE] = STR_OF("Item is too small!"),
    [INVALID_DATA] = STR_OF("Item data is invalid!")};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
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
      THROW(error_self(INCORRECT_ITEM_SIZE));                                  \
                                                                               \
    *result = *(Type *)(item.data + *item_offset);                             \
    *item_offset += sizeof(Type);                                              \
    OK;                                                                        \
  }
READ_IMPL(int32_t, int32)
READ_IMPL(uint64_t, uint64)
READ_IMPL(float, float)

static result_t read_string(item_t item, size_t *item_offset, str_t *result) {
  TRY(str_try_from(item.data + *item_offset, item.size - *item_offset, result));
  CATCH(error, THROW(error))

  *item_offset += str_pack_size((*result));
  OK;
}

static result_t read_bool(item_t item, size_t *item_offset, bool *result) {
  size_t bool_item_size = sizeof(uint8_t);
  if (!verify_size(item, item_offset, bool_item_size))
    THROW(error_self(INCORRECT_ITEM_SIZE));

  uint8_t value = *(uint8_t *)(item.data + *item_offset);
  if (value != false && value != true)
    THROW(error_self(INVALID_DATA));
  *result = (bool)value;
  *item_offset += bool_item_size;
  OK;
}

static void record_insert(struct record *target, size_t index,
                          column_value_t value) {
  target->values[index] = value;
}

static result_t deserialize_column(item_t item, size_t *item_offset,
                                   struct column_schema *column_schema,
                                   size_t column_index, struct record *target) {
  column_value_t column_value = {0};
  switch (column_schema_get_type(column_schema)) {
  case COLUMN_TYPE_UINT64:
    TRY(read_uint64(item, item_offset, &column_value.uint64_value));
    CATCH(error, THROW(error))
    break;
  case COLUMN_TYPE_INT32:
    TRY(read_int32(item, item_offset, &column_value.int32_value));
    CATCH(error, THROW(error))
    break;
  case COLUMN_TYPE_FLOAT:
    TRY(read_float(item, item_offset, &column_value.float_value));
    CATCH(error, THROW(error))
    break;
  case COLUMN_TYPE_STRING: {
    str_t value = STR_NULL;
    TRY(read_string(item, item_offset, &value));
    CATCH(error, THROW(error))

    column_value = (column_value_t){.string_value = str_into(value)};
    break;
  }
  case COLUMN_TYPE_BOOL:
    TRY(read_bool(item, item_offset, &column_value.bool_value));
    CATCH(error, THROW(error))
    break;
  }

  record_insert(target, column_index, column_value);
  OK;
}
result_t record_deserialize(item_t item, struct record *target,
                            size_t start_column_index,
                            size_t end_column_index) {
  ASSERT_NOT_NULL(target, ERROR_SOURCE);

  size_t current_item_offset = 0;
  for (size_t index = start_column_index; index < end_column_index; index++) {
    struct column_schema *schema = target->column_schema_group->schemas[index];

    TRY(deserialize_column(item, &current_item_offset, schema, index, target));
    CATCH(error, THROW(error))
  }

  OK;
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
    *item_size += string_pack_size(column_value.string_value);
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
    string_pack(column_value.string_value, item.data + *item_offset);
    *item_offset += string_pack_size(column_value.string_value);
    break;
  }
  case COLUMN_TYPE_BOOL: {
    *(uint8_t *)(item.data + *item_offset) = column_value.bool_value;
    *item_offset += sizeof(uint8_t);
    break;
  }
  }
}

void record_serialize_into(struct record *record, item_t target) {
  size_t columns_amount = record->column_schema_group->columns_amount;

  size_t item_offset = 0;
  for (size_t index = 0; index < columns_amount; index++) {
    column_type_t type =
        column_schema_get_type(record->column_schema_group->schemas[index]);
    column_value_t value = record->values[index];
    serialize_column(target, &item_offset, type, value);
  }
}

item_t record_serialize(struct record *record) {
  size_t columns_amount = record->column_schema_group->columns_amount;

  size_t item_size = 0;
  for (size_t index = 0; index < columns_amount; index++) {
    column_type_t type =
        column_schema_get_type(record->column_schema_group->schemas[index]);
    column_value_t value = record->values[index];
    calculate_column_size(value, type, &item_size);
  }

  void *item_data = malloc(item_size);
  item_t item = (item_t){.size = item_size, .data = item_data};

  record_serialize_into(record, item);
  return item;
}

// item_t record_serialize(struct record *record) {
//   size_t item_size = 0;
//   struct queue_iterator *it = queue_get_entries(record->entries);
//   while (queue_iterator_has_next(it)) {
//     struct record_entry *entry = NULL;
//     queue_iterator_next(it, (void **)&entry);
//     column_value_t column_value = entry->value;
//     column_type_t column_type = entry->schema.type;
//     calculate_column_size(column_value, column_type, &item_size);
//   }
//   queue_iterator_destroy(it);
//
//   void *item_data = malloc(item_size);
//   item_t item = (item_t){.size = item_size, .data = item_data};
//
//   size_t item_offset = 0;
//   it = queue_get_entries(record->entries);
//   while (queue_iterator_has_next(it)) {
//     struct record_entry *entry = NULL;
//     queue_iterator_next(it, (void **)&entry);
//     column_value_t column_value = entry->value;
//     column_type_t column_type = entry->schema.type;
//     serialize_column(item, &item_offset, column_type, column_value);
//   }
//   queue_iterator_destroy(it);
//
//   return item;
// }