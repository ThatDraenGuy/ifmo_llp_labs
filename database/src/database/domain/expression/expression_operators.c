//
// Created by draen on 03.10.23.
//
#include "database/private/database/domain/expression/expression_operators.h"
#include <malloc.h>

#define ERROR_SOURCE "EXPRESSION_OPERATOR"
#define ERROR_TYPE "EXPRESSION_OPERATOR_ERROR"
enum error_code { NON_NUMERIC_TYPE };

static const char *const error_messages[] = {
    [NON_NUMERIC_TYPE] = "Expression operator expected numeric value!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

#define ARITHMETIC_ON_IMPL(Type, TypeName)                                     \
  Type arithmetic_on_##TypeName(Type first, Type second,                       \
                                arithmetic_operator_t arithmetic_operator) {   \
    switch (arithmetic_operator) {                                             \
                                                                               \
    case PLUS:                                                                 \
      return first + second;                                                   \
    case MINUS:                                                                \
      return first - second;                                                   \
    case MUL:                                                                  \
      return first * second;                                                   \
    case DIV:                                                                  \
      return first / second;                                                   \
    }                                                                          \
  }

ARITHMETIC_ON_IMPL(uint64_t, uint64)
ARITHMETIC_ON_IMPL(int32_t, int32)
ARITHMETIC_ON_IMPL(float, float)

static result_t arithmetic_operator_apply(struct i_expression_operator *oper,
                                          column_value_t first,
                                          column_value_t second,
                                          column_value_t *result) {
  struct arithmetic_operator *self = (struct arithmetic_operator *)oper;

  switch (self->parent.first_type) {

  case COLUMN_TYPE_INT32:
    result->int32_value = arithmetic_on_int32(
        first.int32_value, second.int32_value, self->arithmetic_operator);
    break;
  case COLUMN_TYPE_UINT64:
    result->uint64_value = arithmetic_on_uint64(
        first.uint64_value, second.uint64_value, self->arithmetic_operator);
    break;
  case COLUMN_TYPE_FLOAT:
    result->float_value = arithmetic_on_float(
        first.float_value, second.float_value, self->arithmetic_operator);
    break;
  default:
    THROW(error_self(NON_NUMERIC_TYPE));
  }
  OK;
}
static struct i_expression_operator *
arithmetic_operator_clone(struct i_expression_operator *oper) {
  struct arithmetic_operator *self = (struct arithmetic_operator *)oper;
  return arithmetic_operator(self->arithmetic_operator,
                             self->parent.first_type);
}

static void arithmetic_operator_destroy(struct i_expression_operator *oper) {
  struct arithmetic_operator *self = (struct arithmetic_operator *)oper;
  free(self);
}

struct i_expression_operator *
arithmetic_operator(arithmetic_operator_t arithmetic_operator,
                    column_type_t operand_type) {
  struct arithmetic_operator *oper = malloc(sizeof(struct arithmetic_operator));
  oper->arithmetic_operator = arithmetic_operator;

  oper->parent.first_type = operand_type;
  oper->parent.second_type = operand_type;
  oper->parent.result_type = operand_type;
  oper->parent.apply_impl = arithmetic_operator_apply;
  oper->parent.clone_impl = arithmetic_operator_clone;
  oper->parent.destroy_impl = arithmetic_operator_destroy;
  return (struct i_expression_operator *)oper;
}

#define COMPARE_IMPL(Type, TypeName, FirstValue, SecondValue)                  \
  bool compare_##TypeName(Type first, Type second,                             \
                          comparison_operator_t comparison_operator) {         \
    switch (comparison_operator) {                                             \
    case EQ:                                                                   \
      return FirstValue == SecondValue;                                        \
    case NEQ:                                                                  \
      return FirstValue != SecondValue;                                        \
    case LESS:                                                                 \
      return FirstValue < SecondValue;                                         \
    case LEQ:                                                                  \
      return FirstValue <= SecondValue;                                        \
    case GREATER:                                                              \
      return FirstValue > SecondValue;                                         \
    case GEQ:                                                                  \
      return FirstValue >= SecondValue;                                        \
    }                                                                          \
  }

COMPARE_IMPL(uint64_t, uint64, first, second)
COMPARE_IMPL(int32_t, int32, first, second)
COMPARE_IMPL(float, float, first, second)
COMPARE_IMPL(bool, bool, first, second)
COMPARE_IMPL(str_t, string, str_compare(first, second), 0)

static result_t comparison_operator_apply(struct i_expression_operator *oper,
                                          column_value_t first,
                                          column_value_t second,
                                          column_value_t *result) {
  struct comparison_operator *self = (struct comparison_operator *)oper;

  switch (self->parent.first_type) {

  case COLUMN_TYPE_INT32:
    result->bool_value = compare_int32(first.int32_value, second.int32_value,
                                       self->comparison_operator);
    break;
  case COLUMN_TYPE_UINT64:
    result->bool_value = compare_uint64(first.uint64_value, second.uint64_value,
                                        self->comparison_operator);
    break;
  case COLUMN_TYPE_FLOAT:
    result->bool_value = compare_float(first.float_value, second.float_value,
                                       self->comparison_operator);
    break;
  case COLUMN_TYPE_STRING:
    result->bool_value = compare_string(string_as_str(first.string_value),
                                        string_as_str(second.string_value),
                                        self->comparison_operator);
    break;
  case COLUMN_TYPE_BOOL:
    result->bool_value = compare_bool(first.bool_value, second.bool_value,
                                      self->comparison_operator);
    break;
  }
  OK;
}

static struct i_expression_operator *
comparison_operator_clone(struct i_expression_operator *oper) {
  struct comparison_operator *self = (struct comparison_operator *)oper;
  return comparison_operator(self->comparison_operator,
                             self->parent.first_type);
}

static void comparison_operator_destroy(struct i_expression_operator *oper) {
  struct comparison_operator *self = (struct comparison_operator *)oper;
  free(self);
}

struct i_expression_operator *
comparison_operator(comparison_operator_t comparison_operator,
                    column_type_t operand_type) {
  struct comparison_operator *oper = malloc(sizeof(struct comparison_operator));
  oper->comparison_operator = comparison_operator;

  oper->parent.first_type = operand_type;
  oper->parent.second_type = operand_type;
  oper->parent.result_type = COLUMN_TYPE_BOOL;
  oper->parent.apply_impl = comparison_operator_apply;
  oper->parent.clone_impl = comparison_operator_clone;
  oper->parent.destroy_impl = comparison_operator_destroy;
  return (struct i_expression_operator *)oper;
}

static result_t logical_operator_apply(struct i_expression_operator *oper,
                                       column_value_t first,
                                       column_value_t second,
                                       column_value_t *result) {
  struct logical_operator *self = (struct logical_operator *)oper;

  switch (self->logical_operator) {

  case AND:
    result->bool_value = first.bool_value && second.bool_value;
    break;
  case OR:
    result->bool_value = first.bool_value || second.bool_value;
    break;
  case NOT:
    result->bool_value = !first.bool_value;
    break;
  }
  OK;
}

static struct i_expression_operator *
logical_operator_clone(struct i_expression_operator *oper) {
  struct logical_operator *self = (struct logical_operator *)oper;
  return logical_operator(self->logical_operator);
}

static void logical_operator_destroy(struct i_expression_operator *oper) {
  struct logical_operator *self = (struct logical_operator *)oper;
  free(self);
}

struct i_expression_operator *
logical_operator(logical_operator_t logical_operator) {
  struct logical_operator *oper = malloc(sizeof(struct logical_operator));
  oper->logical_operator = logical_operator;

  oper->parent.first_type = COLUMN_TYPE_BOOL;
  oper->parent.second_type = COLUMN_TYPE_BOOL;
  oper->parent.result_type = COLUMN_TYPE_BOOL;
  oper->parent.apply_impl = logical_operator_apply;
  oper->parent.destroy_impl = logical_operator_destroy;
  return (struct i_expression_operator *)oper;
}