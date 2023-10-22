//
// Created by draen on 03.10.23.
//

#include "database/private/database/domain/expression/expressions.h"
#include <malloc.h>

#define ERROR_SOURCE "EXPRESSION"
#define ERROR_TYPE "EXPRESSION_ERROR"
enum error_code { NON_MATCHING_TYPES };

static const char *const error_messages[] = {
    [NON_MATCHING_TYPES] = "Expressions have different return types!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static void literal_expression_destroy(struct i_expression *expression) {
  struct literal_expression *self = (struct literal_expression *)expression;
  if (self->parent.type == COLUMN_TYPE_STRING)
    string_destroy(self->value.string_value);
  free(self);
}

static struct i_expression *literal_new(column_value_t value,
                                        column_type_t type);

static struct i_expression *
literal_expression_clone(struct i_expression *predicate_value) {
  struct literal_expression *self =
      (struct literal_expression *)predicate_value;
  return literal_new(self->value, self->parent.type);
}

static result_t
literal_expression_get(struct i_expression *expression,
                       __attribute__((unused)) struct record *record,
                       column_value_t *result) {
  struct literal_expression *self = (struct literal_expression *)expression;
  *result = self->value;
  OK;
}

static struct i_expression *literal_new(column_value_t value,
                                        column_type_t type) {
  struct literal_expression *expr = malloc(sizeof(struct literal_expression));

  expr->value = value;

  expr->parent.type = type;
  expr->parent.get_impl = literal_expression_get;
  expr->parent.clone_impl = literal_expression_clone;
  expr->parent.destroy_impl = literal_expression_destroy;
  return (struct i_expression *)expr;
}

#define LITERAL_IMPL(Type, TypeName, ColumnTypeValue)                          \
  struct i_expression *literal_expr_##TypeName(Type value) {                   \
    column_value_t column_value = (column_value_t){.TypeName##_value = value}; \
    return literal_new(column_value, ColumnTypeValue);                         \
  }

LITERAL_IMPL(uint64_t, uint64, COLUMN_TYPE_UINT64)
LITERAL_IMPL(int32_t, int32, COLUMN_TYPE_INT32)
LITERAL_IMPL(bool, bool, COLUMN_TYPE_BOOL)
LITERAL_IMPL(float, float, COLUMN_TYPE_FLOAT)

struct i_expression *literal_expr_string(str_t value) {
  column_value_t column_value =
      (column_value_t){.string_value = str_into(value)};
  return literal_new(column_value, COLUMN_TYPE_STRING);
}

static void column_expression_destroy(struct i_expression *expression) {
  struct column_expression *self = (struct column_expression *)expression;
  free(self);
}

static struct i_expression *
column_expression_clone(struct i_expression *expression) {
  struct column_expression *self = (struct column_expression *)expression;

  return column_expr(self->table_name, self->column_name, self->parent.type);
}

static result_t column_expression_get(struct i_expression *expression,
                                      struct record *record,
                                      column_value_t *result) {
  struct column_expression *self = (struct column_expression *)expression;

  return record_get_value(record, self->table_name, self->column_name, result);
}

struct i_expression *column_expr(str_t table_name, str_t column_name,
                                 column_type_t column_type) {
  struct column_expression *expr = malloc(sizeof(struct column_expression));

  expr->table_name = table_name;
  expr->column_name = column_name;

  expr->parent.type = column_type;
  expr->parent.get_impl = column_expression_get;
  expr->parent.clone_impl = column_expression_clone;
  expr->parent.destroy_impl = column_expression_destroy;
  return (struct i_expression *)expr;
}

static void operator_expression_destroy(struct i_expression *expression) {
  struct operator_expression *self = (struct operator_expression *)expression;
  expression_destroy(self->first);
  expression_destroy(self->second);
  expression_operator_destroy(self->expression_operator);
  free(self);
}

static struct i_expression *
operator_expression_clone(struct i_expression *expression) {
  struct operator_expression *self = (struct operator_expression *)expression;
  return expr_of(expression_clone(self->first), expression_clone(self->second),
                 expression_operator_clone(self->expression_operator));
}

static result_t operator_expression_get(struct i_expression *expression,
                                        struct record *record,
                                        column_value_t *result) {
  struct operator_expression *self = (struct operator_expression *)expression;

  column_type_t first_type = expression_get_type(self->first);
  if (first_type !=
      expression_operator_get_first_operand_type(self->expression_operator))
    THROW(error_self(NON_MATCHING_TYPES));

  column_type_t second_type = expression_get_type(self->second);
  if (second_type !=
      expression_operator_get_second_operand_type(self->expression_operator))
    THROW(error_self(NON_MATCHING_TYPES));

  column_value_t first_value = {0};
  TRY(expression_get(self->first, record, &first_value));
  CATCH(error, THROW(error))

  column_value_t second_value = {0};
  TRY(expression_get(self->second, record, &second_value));
  CATCH(error, THROW(error))

  return expression_operator_apply(self->expression_operator, first_value,
                                   second_value, result);
}

struct i_expression *
expr_of(struct i_expression *first, struct i_expression *second,
        struct i_expression_operator *expression_operator) {
  struct operator_expression *expr = malloc(sizeof(struct operator_expression));
  expr->first = first;
  expr->second = second;
  expr->expression_operator = expression_operator;

  expr->parent.type =
      expression_operator_get_result_operand_type(expression_operator);
  expr->parent.destroy_impl = operator_expression_destroy;
  expr->parent.clone_impl = operator_expression_clone;
  expr->parent.get_impl = operator_expression_get;
  return (struct i_expression *)expr;
}