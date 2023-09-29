//
// Created by draen on 27.09.23.
//

#include "private/database/domain/record_group.h"
#include "private/database/domain/record.h"
#include "public/database/domain/schema.h"
#include "public/database/domain/table.h"
#include "public/error/errors_common.h"
#include "public/util/string.h"
#include <malloc.h>
#include <stdarg.h>

#define ERROR_SOURCE "RECORD_VIEW"

struct record_group *record_group_new() {
  return malloc(sizeof(struct record_group));
}

static void record_values_clear(void *record_values) {
  struct record *self = (struct record *)record_values;
  record_clear_all(self);
}

result_t record_group_ctor(struct record_group *self, size_t table_schema_num,
                           ...) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  va_list args;
  va_start(args, table_schema_num);
  self->column_schema_group.columns_amount = 0;
  for (size_t index = 0; index < table_schema_num; index++) {
    struct table_schema *table_schema = va_arg(args, struct table_schema *);
    self->column_schema_group.columns_amount +=
        table_schema_get_column_amount(table_schema);
  }
  va_end(args);

  self->column_schema_group.schemas =
      malloc(sizeof(void *) * self->column_schema_group.columns_amount);

  va_start(args, table_schema_num);
  size_t column_index = 0;
  for (size_t table_index = 0; table_index < table_schema_num; table_index++) {
    struct table_schema *table = va_arg(args, struct table_schema *);
    struct column_schema_iterator *it = table_schema_get_columns(table);

    while (column_schema_iterator_has_next(it)) {
      column_schema_iterator_next(
          it, &self->column_schema_group.schemas[column_index]);
      column_index++;
    }
    column_schema_iterator_destroy(it);
  }
  va_end(args);

  self->records = queue_new();
  queue_ctor(self->records,
             sizeof(struct record) +
                 sizeof(column_value_t) *
                     self->column_schema_group.columns_amount,
             record_values_clear);

  OK;
}

result_t record_group_insert(struct record_group *self, size_t values_num,
                             ...) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (self->column_schema_group.columns_amount != values_num) {
    // TODO throw error
  }

  struct record *record = queue_push_back(self->records, NULL);
  record->column_schema_group = &self->column_schema_group;

  va_list args;
  va_start(args, values_num);
  for (size_t index = 0; index < values_num; index++) {
    column_type_t arg_type = va_arg(args, column_type_t);

    struct column_schema *schema = self->column_schema_group.schemas[index];
    if (arg_type != column_schema_get_type(schema)) {
      // TODO throw error
    }

    column_value_t *value = &record->values[index];
    switch (column_schema_get_type(schema)) {
    case COLUMN_TYPE_INT32:
      *value = (column_value_t){.int32_value = va_arg(args, int32_t)};
      break;
    case COLUMN_TYPE_UINT64:
      *value = (column_value_t){.uint64_value = va_arg(args, uint64_t)};
      break;
    case COLUMN_TYPE_FLOAT:
      *value = (column_value_t){.float_value = va_arg(args, double)};
      break;
    case COLUMN_TYPE_STRING:
      *value = (column_value_t){.string_value = str_into(va_arg(args, str_t))};
      break;
    case COLUMN_TYPE_BOOL:
      *value = (column_value_t){.bool_value = va_arg(args, int)};
      break;
    }
  }
  va_end(args);

  OK;
}

void record_group_clear(struct record_group *self) {
  queue_clear(self->records);
}

struct record_iterator *record_group_get_records(struct record_group *self) {
  struct record_iterator *it = malloc(sizeof(struct record_iterator));
  it->it = queue_get_entries(self->records);
  return it;
}

bool record_iterator_has_next(struct record_iterator *self) {
  return queue_iterator_has_next(self->it);
}

result_t record_iterator_next(struct record_iterator *self,
                              struct record **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  if (!record_iterator_has_next(self))
    THROW(error_common(ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  return queue_iterator_next(self->it, (void **)result);
}

void record_iterator_destroy(struct record_iterator *self) {
  queue_iterator_destroy(self->it);
  free(self);
}

void record_group_destroy(struct record_group *self) {
  queue_destroy(self->records);
  free(self->column_schema_group.schemas);
  free(self);
}
