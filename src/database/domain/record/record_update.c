//
// Created by draen on 30.09.23.
//

#include "private/database/domain/record/record_update.h"
#include "private/database/domain/record/record.h"
#include "private/database/domain/schema.h"
#include <malloc.h>
#include <stdarg.h>

struct record_update *record_update_new() {
  return malloc(sizeof(struct record_update));
}

void record_update_ctor(struct record_update *self, str_t table_name,
                        size_t columns_num, ...) {
  self->columns_num = columns_num;
  self->table_name = str_into(table_name);
  self->applicable_in_place = true;

  va_list args;
  va_start(args, columns_num);
  self->schemas = malloc(sizeof(void *) * columns_num);
  self->expressions = malloc(sizeof(void *) * columns_num);
  for (size_t index = 0; index < columns_num; index++) {
    str_t column_name = va_arg(args, str_t);
    struct i_expression *expression = va_arg(args, struct i_expression *);
    self->expressions[index] = expression;
    self->schemas[index] = malloc(sizeof(struct column_schema));
    self->schemas[index]->type = expression_get_type(expression);
    self->schemas[index]->name = str_into(column_name);
    self->schemas[index]->table_name = string_as_str(self->table_name);
  }
  va_end(args);
}

result_t record_update_apply(struct record_update *self,
                             struct record *target) {
  for (size_t index = 0; index < self->columns_num; index++) {
    struct column_schema *schema = self->schemas[index];
    struct i_expression *expression = self->expressions[index];
    column_value_t result_value = {0};
    TRY(expression_get(expression, target, &result_value));
    CATCH(error, THROW(error))

    TRY(record_set_value(target, column_schema_get_table_name(schema),
                         column_schema_get_name(schema), result_value));
    CATCH(error, THROW(error))
  }
  OK;
}

void record_update_destroy(struct record_update *self) {
  for (size_t index = 0; index < self->columns_num; index++) {
    string_destroy(self->schemas[index]->name);
    free(self->schemas[index]);

    expression_destroy(self->expressions[index]);
  }
  free(self->schemas);
  free(self->expressions);
  string_destroy(self->table_name);
  free(self);
}