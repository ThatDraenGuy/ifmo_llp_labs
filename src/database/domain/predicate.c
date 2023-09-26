//
// Created by draen on 19.09.23.
//

#include "private/database/domain/predicate.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <string.h>

#define ERROR_SOURCE "PREDICATE"
#define ERROR_TYPE "PREDICATE_ERROR"
enum error_code { NON_MATCHING_TYPES };

static const char *const error_messages[] = {
    [NON_MATCHING_TYPES] = "Predicate values have different types!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static void
literal_predicate_value_destroy(struct predicate_value *predicate_value) {
  struct literal_predicate_value *self =
      (struct literal_predicate_value *)predicate_value;
  free(self);
}

static struct predicate_value *literal_new(column_value_t value,
                                           column_type_t type);

static struct predicate_value *
literal_predicate_value_clone(struct predicate_value *predicate_value) {
  struct literal_predicate_value *self =
      (struct literal_predicate_value *)predicate_value;
  return literal_new(self->value, self->parent.type);
}

static result_t
literal_predicate_value_get_value(struct predicate_value *predicate_value,
                                  __attribute__((unused)) struct record *record,
                                  column_value_t *result) {
  struct literal_predicate_value *self =
      (struct literal_predicate_value *)predicate_value;
  *result = self->value;
  OK;
}

static struct predicate_value *literal_new(column_value_t value,
                                           column_type_t type) {
  struct literal_predicate_value *self =
      malloc(sizeof(struct literal_predicate_value));

  self->value = value;

  self->parent.type = type;
  self->parent.get_value_impl = literal_predicate_value_get_value;
  self->parent.clone_impl = literal_predicate_value_clone;
  self->parent.destroy_impl = literal_predicate_value_destroy;
  return (struct predicate_value *)self;
}

#define LITERAL_IMPL(Type, TypeName, ColumnTypeValue)                          \
  struct predicate_value *literal_##TypeName(Type value) {                     \
    column_value_t column_value = (column_value_t){.TypeName##_value = value}; \
    return literal_new(column_value, ColumnTypeValue);                         \
  }

LITERAL_IMPL(uint64_t, uint64, COLUMN_TYPE_UINT64);
LITERAL_IMPL(int32_t, int32, COLUMN_TYPE_INT32);
LITERAL_IMPL(char *, string, COLUMN_TYPE_STRING);
LITERAL_IMPL(bool, bool, COLUMN_TYPE_BOOL);
LITERAL_IMPL(float, float, COLUMN_TYPE_FLOAT);

static void
column_predicate_value_destroy(struct predicate_value *predicate_value) {
  struct column_predicate_value *self =
      (struct column_predicate_value *)predicate_value;
  free(self->column_name);
  free(self);
}

static struct predicate_value *
column_predicate_value_clone(struct predicate_value *predicate_value) {
  struct column_predicate_value *self =
      (struct column_predicate_value *)predicate_value;

  return column_value(self->column_name, self->parent.type);
}

static result_t
column_predicate_value_get_value(struct predicate_value *predicate_value,
                                 struct record *record,
                                 column_value_t *result) {
  struct column_predicate_value *self =
      (struct column_predicate_value *)predicate_value;

  return record_get_value(record, self->column_name, result);
}

struct predicate_value *column_value(char *column_name,
                                     column_type_t column_type) {
  struct column_predicate_value *predicate_value =
      malloc(sizeof(struct column_predicate_value));

  predicate_value->column_name = malloc(strlen(column_name) + 1);
  strcpy(predicate_value->column_name, column_name);

  predicate_value->parent.type = column_type;
  predicate_value->parent.get_value_impl = column_predicate_value_get_value;
  predicate_value->parent.clone_impl = column_predicate_value_clone;
  predicate_value->parent.destroy_impl = column_predicate_value_destroy;
  return (struct predicate_value *)predicate_value;
}

result_t predicate_apply(struct predicate *self, struct record *record,
                         bool *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  return self->apply_impl(self, record, result);
}

struct predicate *predicate_clone(struct predicate *self) {
  if (self == NULL)
    return NULL;

  return self->clone_impl(self);
}

void predicate_destroy(struct predicate *self) { self->destroy_impl(self); }

#define compare_number_impl(Type, TypeName)                                    \
  static result_t compare_##TypeName(                                          \
      Type first, Type second, comparison_operator_t comparison_operator,      \
      bool *result) {                                                          \
    switch (comparison_operator) {                                             \
    case EQ: {                                                                 \
      *result = first == second;                                               \
      OK;                                                                      \
    }                                                                          \
    case NEQ: {                                                                \
      *result = first != second;                                               \
      OK;                                                                      \
    }                                                                          \
    case LESS: {                                                               \
      *result = first < second;                                                \
      OK;                                                                      \
    }                                                                          \
    case LEQ: {                                                                \
      *result = first <= second;                                               \
      OK;                                                                      \
    }                                                                          \
    case GREATER: {                                                            \
      *result = first > second;                                                \
      OK;                                                                      \
    }                                                                          \
    case GEQ: {                                                                \
      *result - first >= second;                                               \
      OK;                                                                      \
    }                                                                          \
    }                                                                          \
  }
compare_number_impl(uint64_t, uint64);
compare_number_impl(int32_t, int32);
compare_number_impl(float, float); // TODO maybe redo
compare_number_impl(bool, bool);

static result_t compare_string(char *first, char *second,
                               comparison_operator_t comparison_operator,
                               bool *result) {
  switch (comparison_operator) {
  case EQ: {
    *result = strcmp(first, second) == 0;
    break;
  }
  case NEQ: {
    *result = strcmp(first, second) != 0;
    break;
  }
  case LESS: {
    *result = strcmp(first, second) < 0;
    break;
  }
  case LEQ: {
    *result = strcmp(first, second) <= 0;
    break;
  }
  case GREATER: {
    *result = strcmp(first, second) > 0;
    break;
  }
  case GEQ: {
    *result = strcmp(first, second) >= 0;
    break;
  }
  }
  OK;
}

static result_t predicate_of_impl_apply(struct predicate *predicate,
                                        struct record *record, bool *result) {
  struct predicate_of_impl *self = (struct predicate_of_impl *)predicate;

  column_type_t first_type = self->first->type;
  column_type_t second_type = self->second->type;
  if (first_type != second_type)
    THROW(error_self(NON_MATCHING_TYPES));

  column_value_t first_value = {0};
  TRY(self->first->get_value_impl(self->first, record, &first_value));
  CATCH(error, THROW(error))

  column_value_t second_value = {0};
  TRY(self->second->get_value_impl(self->second, record, &second_value));
  CATCH(error, THROW(error))

  switch (first_type) {
  case COLUMN_TYPE_UINT64:
    return compare_uint64(first_value.uint64_value, second_value.uint64_value,
                          self->comparison_operator, result);
  case COLUMN_TYPE_INT32:
    return compare_int32(first_value.int32_value, second_value.int32_value,
                         self->comparison_operator, result);
  case COLUMN_TYPE_FLOAT:
    return compare_float(first_value.float_value, second_value.float_value,
                         self->comparison_operator, result);
  case COLUMN_TYPE_BOOL:
    return compare_bool(first_value.bool_value, second_value.bool_value,
                        self->comparison_operator, result);
  case COLUMN_TYPE_STRING:
    return compare_string(first_value.string_value, second_value.string_value,
                          self->comparison_operator, result);
  }
}

static struct predicate *predicate_of_impl_clone(struct predicate *predicate) {
  struct predicate_of_impl *self = (struct predicate_of_impl *)predicate;
  return predicate_of(self->first->clone_impl(self->first),
                      self->second->clone_impl(self->second),
                      self->comparison_operator);
}

static void predicate_of_impl_destroy(struct predicate *predicate) {
  struct predicate_of_impl *self = (struct predicate_of_impl *)predicate;
  self->first->destroy_impl(self->first);
  self->second->destroy_impl(self->second);
  free(self);
}

struct predicate *predicate_of(struct predicate_value *first,
                               struct predicate_value *second,
                               comparison_operator_t comparison_operator) {
  struct predicate_of_impl *predicate =
      malloc(sizeof(struct predicate_of_impl));
  predicate->first = first;
  predicate->second = second;
  predicate->comparison_operator = comparison_operator;

  predicate->parent.apply_impl = predicate_of_impl_apply;
  predicate->parent.clone_impl = predicate_of_impl_clone;
  predicate->parent.destroy_impl = predicate_of_impl_destroy;
  return (struct predicate *)predicate;
}

static result_t predicate_logical_impl_apply(struct predicate *predicate,
                                             struct record *record,
                                             bool *result) {
  struct predicate_logical_impl *self =
      (struct predicate_logical_impl *)predicate;

  switch (self->logical_operator) {

  case AND: {
    bool first = false;
    TRY(self->first->apply_impl(self->first, record, &first));
    CATCH(error, THROW(error))

    bool second = false;
    TRY(self->second->apply_impl(self->second, record, &second));
    CATCH(error, THROW(error))

    *result = first && second;
    OK;
  }
  case OR: {
    bool first = false;
    TRY(self->first->apply_impl(self->first, record, &first));
    CATCH(error, THROW(error))

    bool second = false;
    TRY(self->second->apply_impl(self->second, record, &second));
    CATCH(error, THROW(error))

    *result = first || second;
    OK;
  }
  case NOT: {
    bool first = false;
    TRY(self->first->apply_impl(self->first, record, &first));
    CATCH(error, THROW(error))

    *result = !first;
    OK;
  }
  }
}

static struct predicate *
predicate_logical_impl_new(struct predicate *first, struct predicate *second,
                           logical_operator_t logical_operator);
static struct predicate *
predicate_logical_impl_clone(struct predicate *predicate) {
  struct predicate_logical_impl *self =
      (struct predicate_logical_impl *)predicate;

  return predicate_logical_impl_new(predicate_clone(self->first),
                                    predicate_clone(self->second),
                                    self->logical_operator);
}

static void predicate_logical_impl_destroy(struct predicate *predicate) {
  struct predicate_logical_impl *self =
      (struct predicate_logical_impl *)predicate;
  self->first->destroy_impl(self->first);
  self->second->destroy_impl(self->second);
  free(self);
}

static struct predicate *
predicate_logical_impl_new(struct predicate *first, struct predicate *second,
                           logical_operator_t logical_operator) {
  struct predicate_logical_impl *predicate =
      malloc(sizeof(struct predicate_logical_impl));
  predicate->first = first;
  predicate->second = second;
  predicate->logical_operator = AND;

  predicate->parent.apply_impl = predicate_logical_impl_apply;
  predicate->parent.clone_impl = predicate_logical_impl_clone;
  predicate->parent.destroy_impl = predicate_of_impl_destroy;
  return (struct predicate *)predicate;
}

struct predicate *predicate_and(struct predicate *first,
                                struct predicate *second) {
  return predicate_logical_impl_new(first, second, AND);
}
struct predicate *predicate_or(struct predicate *first,
                               struct predicate *second) {
  return predicate_logical_impl_new(first, second, OR);
}
struct predicate *predicate_not(struct predicate *predicate) {
  return predicate_logical_impl_new(predicate, NULL, NOT);
}