//
// Created by draen on 30.09.23.
//

#include "private/database/domain/record_update.h"
#include "private/database/domain/record.h"
#include "private/database/domain/schema.h"
#include <malloc.h>
#include <stdarg.h>

struct record_update *record_update_new() {
  return malloc(sizeof(struct record_update));
}

void record_update_ctor(struct record_update *self, str_t table_name,
                        size_t columns_num, ...) {
  self->update_holder = single_record_holder_new();
  self->columns_num = columns_num;
  self->table_name = str_into(table_name);
  self->applicable_in_place = true;

  va_list args;
  va_start(args, columns_num);
  self->schemas = malloc(sizeof(void *) * columns_num);

  for (size_t index = 0; index < columns_num; index++) {
    str_t column_name = va_arg(args, str_t);
    column_type_t column_type = va_arg(args, column_type_t);
    va_arg(args, void *);
    self->schemas[index] = malloc(sizeof(struct column_schema));
    self->schemas[index]->type = column_type;
    self->schemas[index]->name = str_into(column_name);
    self->schemas[index]->table_name = string_as_str(self->table_name);

    if (column_type == COLUMN_TYPE_STRING)
      self->applicable_in_place = false;
  }
  va_end(args);
  va_start(args, columns_num);
  single_record_holder_ctor_from_columns(self->update_holder, columns_num,
                                         self->schemas);
  for (size_t index = 0; index < columns_num; index++) {
    va_arg(args, void *);
    column_type_t column_type = va_arg(args, column_type_t);
    column_value_t column_value;
    if (column_type == COLUMN_TYPE_STRING)
      column_value =
          (column_value_t){.string_value = str_into(va_arg(args, str_t))};
    else if (column_type == COLUMN_TYPE_FLOAT) {
      double val = va_arg(args, double);
      column_value = (column_value_t){.float_value = (float)val};
    } else
      column_value = va_arg(args, column_value_t);
    single_record_holder_get(self->update_holder)->values[index] = column_value;
  }
  va_end(args);
}

result_t record_update_apply(struct record_update *self,
                             struct record *target) {
  for (size_t index = 0; index < self->columns_num; index++) {
    struct column_schema *schema = self->schemas[index];
    column_value_t value =
        single_record_holder_get(self->update_holder)->values[index];

    TRY(record_set_value(target, column_schema_get_table_name(schema),
                         column_schema_get_name(schema), value));
    CATCH(error, THROW(error))
  }
  OK;
}

void record_update_destroy(struct record_update *self) {
  single_record_holder_destroy(self->update_holder);
  for (size_t index = 0; index < self->columns_num; index++) {
    string_destroy(self->schemas[index]->name);
    free(self->schemas[index]);
  }
  free(self->schemas);
  string_destroy(self->table_name);
  free(self);
}