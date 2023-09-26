//
// Created by draen on 18.09.23.
//

#include "private/database/domain/record.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <string.h>

#define ERROR_SOURCE "RECORD"
#define ERROR_TYPE "RECORD_ERROR"

enum error_code {
  INCORRECT_COLUMN_TYPE,
  UNKNOWN_COLUMN_NAME,
};

static const char *const error_messages[] = {
    [INCORRECT_COLUMN_TYPE] = "Incorrect column type!",
    [UNKNOWN_COLUMN_NAME] = "Unknown column name!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

struct record *record_new() { return malloc(sizeof(struct record)); }

static void record_entry_destroy(void *entry) {
  struct record_entry *record_entry = (struct record_entry *)entry;
  if (record_entry->schema.type == COLUMN_TYPE_STRING)
    free(record_entry->value.string_value);
}

void record_ctor(struct record *self) {
  //  self->column_amount = column_amount;
  //  self->current_entry_index = 0;
  //  self->entries = malloc(sizeof(struct record_entry) * column_amount);
  self->entries = queue_new();
  queue_ctor(self->entries, sizeof(struct record_entry), record_entry_destroy);
}

void record_clear(struct record *self) {
  queue_destroy(self->entries);
  self->entries = queue_new();
  queue_ctor(self->entries, sizeof(struct record_entry), record_entry_destroy);
}

result_t record_insert_value(struct record *self, char *column_name,
                             column_value_t value, column_type_t type) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  struct record_entry *new_entry = queue_push_back(self->entries, NULL);
  new_entry->schema.name = column_name;
  new_entry->schema.type = type;
  new_entry->value = value;
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
                              char *value) {
  size_t string_size = strlen(value) + 1;
  char *actual_value = malloc(string_size);
  strcpy(actual_value, value);
  return record_insert_value(self, column_name,
                             (column_value_t){.string_value = actual_value},
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
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  return queue_get(self->entries, column_index, (void **)result);
}

result_t record_copy_into(struct record *self, struct record *target) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  struct queue_iterator *it = queue_get_entries(self->entries);
  while (queue_iterator_has_next(it)) {
    struct record_entry *entry = NULL;
    TRY(queue_iterator_next(it, (void **)&entry));
    CATCH(error, {
      queue_iterator_destroy(it);
      THROW(error);
    })

    if (entry->schema.type == COLUMN_TYPE_STRING) {
      TRY(record_insert_string(target, entry->schema.name,
                               entry->value.string_value));
      CATCH(error, {
        queue_iterator_destroy(it);
        THROW(error);
      })
    } else {
      TRY(record_insert_value(target, entry->schema.name, entry->value,
                              entry->schema.type));
      CATCH(error, {
        queue_iterator_destroy(it);
        THROW(error);
      })
    }
  }
  queue_iterator_destroy(it);
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
    ASSERT_NOT_NULL(self, ERROR_SOURCE);                                       \
                                                                               \
    struct record_entry *entry = NULL;                                         \
    TRY(get_entry_at(self, column_index, &entry));                             \
    CATCH(error, THROW(error))                                                 \
                                                                               \
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
  struct queue_iterator *it = queue_get_entries(self->entries);
  while (queue_iterator_has_next(it)) {
    struct record_entry *entry = NULL;
    TRY(queue_iterator_next(it, (void **)&entry));
    CATCH(error, {
      queue_iterator_destroy(it);
      return NULL;
    })
    if (strcmp(entry->schema.name, column_name) == 0) {
      queue_iterator_destroy(it);
      return entry;
    }
  }

  return NULL;
}

result_t record_get_value(struct record *self, char *column_name,
                          column_value_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct record_entry *entry = find_entry(self, column_name);
  if (entry == NULL)
    THROW(error_self(UNKNOWN_COLUMN_NAME));

  *result = entry->value;
  OK;
}

#define RECORD_GET_IMPL(Type, TypeName, ColumnTypeValue)                       \
  result_t record_get_##TypeName(struct record *self, char *column_name,       \
                                 Type *result) {                               \
    ASSERT_NOT_NULL(self, ERROR_SOURCE);                                       \
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
  queue_destroy(self->entries);
  free(self);
}