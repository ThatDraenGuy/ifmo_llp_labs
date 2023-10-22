//
// Created by draen on 19.09.23.
//

#include "database/private/database/domain/predicate.h"
#include "common/public/error/errors_common.h"
#include <malloc.h>

#define ERROR_SOURCE "PREDICATE"
#define ERROR_TYPE "PREDICATE_ERROR"
enum error_code { NON_BOOL_EXPRESSION };

static const char *const error_messages[] = {
    [NON_BOOL_EXPRESSION] =
        "Predicate expected to have a BOOL-type expression!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

result_t predicate_apply(struct predicate *self, struct record *record,
                         bool *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (expression_get_type(self->expression) != COLUMN_TYPE_BOOL)
    THROW(error_self(NON_BOOL_EXPRESSION));

  column_value_t result_value = {0};
  TRY(expression_get(self->expression, record, &result_value));
  CATCH(error, THROW(error))

  *result = result_value.bool_value;
  OK;
}

struct predicate *predicate_clone(struct predicate *self) {
  if (self == NULL)
    return NULL;

  return predicate_of(expression_clone(self->expression));
}

struct predicate *predicate_of(struct i_expression *expression) {
  struct predicate *self = malloc(sizeof(struct predicate));
  self->expression = expression;
  return self;
}

void predicate_destroy(struct predicate *self) {
  expression_destroy(self->expression);
  free(self);
}