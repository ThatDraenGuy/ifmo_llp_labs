//
// Created by draen on 20.09.23.
//

#include "private/database/domain/schema.h"
#include "public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE "SCHEMA"
#define ERROR_TYPE "SCHEMA_ERROR"
enum error_code { SCHEMA_SIZE_EXCEEDED };

static const char *const error_messages[] = {[SCHEMA_SIZE_EXCEEDED] =
                                                 "Schema size was exceeded!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

column_type_t column_schema_get_type(struct column_schema *self) {
  return self->type;
}

str_t column_schema_get_name(struct column_schema *self) {
  return string_as_str(self->name);
}

str_t column_schema_get_table_name(struct column_schema *self) {
  return self->table_name;
}

struct table_schema *table_schema_new() {
  return malloc(sizeof(struct table_schema));
}

void table_schema_ctor(struct table_schema *self, str_t table_name,
                       size_t columns_amount) {
  self->column_schema_group.columns_amount = columns_amount;
  self->current_column_index = 0;

  self->table_name = str_into(table_name);

  self->column_schema_group.schemas = malloc(sizeof(void *) * columns_amount);
}

str_t table_schema_get_name(struct table_schema *self) {
  return string_as_str(self->table_name);
}

size_t table_schema_get_column_amount(struct table_schema *self) {
  return self->column_schema_group.columns_amount;
}

result_t table_schema_add_column(struct table_schema *self, str_t column_name,
                                 column_type_t column_type) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (self->current_column_index >= self->column_schema_group.columns_amount)
    THROW(error_self(SCHEMA_SIZE_EXCEEDED));

  self->column_schema_group.schemas[self->current_column_index] =
      malloc(sizeof(struct column_schema));
  self->column_schema_group.schemas[self->current_column_index]->name =
      str_into(column_name);
  self->column_schema_group.schemas[self->current_column_index]->type =
      column_type;
  self->column_schema_group.schemas[self->current_column_index]->table_name =
      string_as_str(self->table_name);

  self->current_column_index++;

  OK;
}

void table_schema_destroy(struct table_schema *self) {
  for (size_t index = 0; index < self->current_column_index; index++) {
    string_destroy(self->column_schema_group.schemas[index]->name);
    free(self->column_schema_group.schemas[index]);
  }
  string_destroy(self->table_name);
  free(self->column_schema_group.schemas);
  free(self);
}

struct column_schema_iterator *
table_schema_get_columns(struct table_schema *self) {
  struct column_schema_iterator *it =
      malloc(sizeof(struct column_schema_iterator));

  it->column_schema_group = &self->column_schema_group;
  it->current_index = 0;

  return it;
}

bool column_schema_iterator_has_next(struct column_schema_iterator *self) {
  return self->current_index < self->column_schema_group->columns_amount;
}

result_t column_schema_iterator_next(struct column_schema_iterator *self,
                                     struct column_schema **result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (!column_schema_iterator_has_next(self))
    THROW(error_common(ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  *result = self->column_schema_group->schemas[self->current_index];
  self->current_index++;
  OK;
}

void column_schema_iterator_destroy(struct column_schema_iterator *self) {
  free(self);
}