//
// Created by draen on 20.09.23.
//

#include "private/database/domain/schema.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <string.h>

static const char *const error_source = "SCHEMA";
static const char *const error_type = "SCHEMA_ERROR";
enum error_code { SCHEMA_SIZE_EXCEEDED };

static const char *const error_messages[] = {[SCHEMA_SIZE_EXCEEDED] =
                                                 "Schema size was exceeded!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(error_source, error_type, (error_code_t){error_code},
                   error_messages[error_code]);
}

struct table_schema *table_schema_new() {
  return malloc(sizeof(struct table_schema));
}

void table_schema_ctor(struct table_schema *self, char *table_name,
                       size_t columns_amount) {
  self->column_amount = columns_amount;
  self->current_column_index = 0;

  self->table_name = malloc(strlen(table_name) + 1);
  strcpy(self->table_name, table_name);

  self->columns = malloc(sizeof(struct column_schema) * columns_amount);
}

char *table_schema_get_name(struct table_schema *self) {
  return self->table_name;
}

size_t table_schema_get_column_amount(struct table_schema *self) {
  return self->column_amount;
}

result_t table_schema_add_column(struct table_schema *self, char *column_name,
                                 column_type_t column_type) {
  ASSERT_NOT_NULL(self, error_source);

  if (self->current_column_index >= self->column_amount)
    return result_err(error_self(SCHEMA_SIZE_EXCEEDED));

  self->columns[self->current_column_index].name =
      column_name; // TODO think about strings
  self->columns[self->current_column_index].type = column_type;

  self->current_column_index++;

  return OK;
}

void table_schema_destroy(struct table_schema *self) {
  free(self->table_name);
  free(self->columns);
  free(self);
}

struct column_schema_iterator *
table_schema_get_columns(struct table_schema *self) {
  struct column_schema_iterator *it =
      malloc(sizeof(struct column_schema_iterator));

  it->columns = self->columns;
  it->column_amount = self->current_column_index;
  it->current_index = 0;

  return it;
}

bool column_schema_iterator_has_next(struct column_schema_iterator *self) {
  return self->current_index < self->column_amount;
}

result_t column_schema_iterator_next(struct column_schema_iterator *self,
                                     struct column_schema **result) {
  ASSERT_NOT_NULL(self, error_source);

  if (!column_schema_iterator_has_next(self))
    return result_err(error_common(error_source, ERR_COMMON_ITER_OUT_OF_RANGE));

  *result = &self->columns[self->current_index];
  self->current_index++;
  return OK;
}

void column_schema_iterator_destroy(struct column_schema_iterator *self) {
  free(self);
}