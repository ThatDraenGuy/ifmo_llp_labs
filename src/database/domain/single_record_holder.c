//
// Created by draen on 28.09.23.
//

#include "private/database/domain/single_record_holder.h"
#include "private/database/domain/record.h"
#include "public/database/domain/table.h"
#include <malloc.h>
#include <stdarg.h>

struct single_record_holder *single_record_holder_new() {
  return malloc(sizeof(struct single_record_holder));
}

void single_record_holder_ctor_with_joins(struct single_record_holder *self,
                                          struct table *main_table,
                                          size_t joins_num,
                                          struct table **join_tables) {
  self->column_schema_group.columns_amount =
      table_schema_get_column_amount(table_get_schema(main_table));

  for (size_t index = 0; index < joins_num; index++) {
    self->column_schema_group.columns_amount +=
        table_schema_get_column_amount(table_get_schema(join_tables[index]));
  }

  self->column_schema_group.schemas =
      malloc(sizeof(void *) * self->column_schema_group.columns_amount);

  size_t column_index = 0;
  {
    struct column_schema_iterator *it =
        table_schema_get_columns(table_get_schema(main_table));
    while (column_schema_iterator_has_next(it)) {
      column_schema_iterator_next(
          it, &self->column_schema_group.schemas[column_index]);
      column_index++;
    }
    column_schema_iterator_destroy(it);
  }
  for (size_t index = 0; index < joins_num; index++) {
    struct column_schema_iterator *it =
        table_schema_get_columns(table_get_schema(join_tables[index]));
    while (column_schema_iterator_has_next(it)) {
      column_schema_iterator_next(
          it, &self->column_schema_group.schemas[column_index]);
      column_index++;
    }
    column_schema_iterator_destroy(it);
  }

  self->record = record_new(&self->column_schema_group);
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

void single_record_holder_ctor_from_columns(struct single_record_holder *self,
                                            size_t columns_num,
                                            struct column_schema **schemas) {
  self->column_schema_group.columns_amount = columns_num;
  self->column_schema_group.schemas = malloc(sizeof(void *) * columns_num);
  for (size_t index = 0; index < columns_num; index++) {
    self->column_schema_group.schemas[index] = schemas[index];
  }

  self->record = record_new(&self->column_schema_group);
}

struct record *single_record_holder_get(struct single_record_holder *self) {
  return self->record;
}

void single_record_holder_clear(struct single_record_holder *self,
                                size_t first_column, size_t last_column) {
  record_clear(self->record, first_column, last_column);
}
void single_record_holder_clear_all(struct single_record_holder *self) {
  record_clear_all(self->record);
}

void single_record_holder_destroy(struct single_record_holder *self) {
  record_destroy(self->record);
  free(self->column_schema_group.schemas);
  free(self);
}