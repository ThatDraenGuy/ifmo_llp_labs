//
// Created by draen on 28.09.23.
//

#include "private/database/domain/single_record_holder.h"
#include "private/database/domain/record.h"
#include <malloc.h>
#include <stdarg.h>

struct single_record_holder *single_record_holder_new() {
  return malloc(sizeof(struct single_record_holder));
}

void single_record_holder_ctor(struct single_record_holder *self,
                               size_t table_schema_num, ...) {
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

  self->record = record_new(&self->column_schema_group);
}

struct record *
single_record_holder_get_values(struct single_record_holder *self) {
  return self->record;
}

void single_record_holder_clear(struct single_record_holder *self) {
  record_clear(self->record);
}

void single_record_holder_destroy(struct single_record_holder *self) {
  record_destroy(self->record);
  free(self->column_schema_group.schemas);
  free(self);
}