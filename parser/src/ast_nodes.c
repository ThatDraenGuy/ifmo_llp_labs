//
// Created by draen on 26.10.23.
//

#include "parser/private/ast_nodes.h"
#include <malloc.h>

#define PRINT_INDENT(Level)                                                    \
  for (size_t i = 0; i < Level; i++) {                                         \
    printf("\t");                                                              \
  }

static bool common_ast_node_equals(struct i_ast_node *self,
                                   struct i_ast_node *other) {
  return str_eq(self->name, other->name);
}
static void common_ast_node_destroy(struct i_ast_node *node) { free(node); }

// simple_ast_node
static bool simple_ast_node_equals(struct i_ast_node *node1,
                                   struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct simple_ast_node *self = (struct simple_ast_node *)node1;
  struct simple_ast_node *other = (struct simple_ast_node *)node2;

  return ast_node_equals(self->child, other->child);
}

static void simple_ast_node_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct simple_ast_node *self = (struct simple_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->parent.name._data);
  ast_node_print_at_level(self->child, current_level + 1);
}
static void simple_ast_node_destroy(struct i_ast_node *node) {
  struct simple_ast_node *self = (struct simple_ast_node *)node;

  ast_node_destroy(self->child);
  free(self);
}

static struct simple_ast_node *simple_ast_node_new(struct i_ast_node *child,
                                                   str_t name) {
  struct simple_ast_node *self = malloc(sizeof(struct simple_ast_node));

  self->child = child;
  self->parent.name = name;

  self->parent.equals_impl = simple_ast_node_equals;
  self->parent.print_at_level_impl = simple_ast_node_print;
  self->parent.destroy_impl = simple_ast_node_destroy;
  return self;
}

// double_ast_node
static bool double_ast_node_equals(struct i_ast_node *node1,
                                   struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct double_ast_node *self = (struct double_ast_node *)node1;
  struct double_ast_node *other = (struct double_ast_node *)node2;

  return ast_node_equals(self->left, other->left) &&
         ast_node_equals(self->right, other->right);
}

static void double_ast_node_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct double_ast_node *self = (struct double_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->parent.name._data);

  ast_node_print_at_level(self->left, current_level + 1);
  ast_node_print_at_level(self->right, current_level + 1);
}

static void double_ast_node_destroy(struct i_ast_node *node) {
  struct double_ast_node *self = (struct double_ast_node *)node;

  ast_node_destroy(self->left);
  ast_node_destroy(self->right);
  free(self);
}

static struct double_ast_node *double_ast_node_new(struct i_ast_node *left,
                                                   struct i_ast_node *right,
                                                   str_t name) {
  struct double_ast_node *self = malloc(sizeof(struct double_ast_node));

  self->left = left;
  self->right = right;
  self->parent.name = name;

  self->parent.equals_impl = double_ast_node_equals;
  self->parent.print_at_level_impl = double_ast_node_print;
  self->parent.destroy_impl = double_ast_node_destroy;
  return self;
}

// triple_ast_node
static bool triple_ast_node_equals(struct i_ast_node *node1,
                                   struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct triple_ast_node *self = (struct triple_ast_node *)node1;
  struct triple_ast_node *other = (struct triple_ast_node *)node2;

  return ast_node_equals(self->left, other->left) &&
         ast_node_equals(self->mid, other->mid) &&
         ast_node_equals(self->right, other->right);
}

static void triple_ast_node_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct triple_ast_node *self = (struct triple_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->parent.name._data);

  ast_node_print_at_level(self->left, current_level + 1);
  ast_node_print_at_level(self->mid, current_level + 1);
  ast_node_print_at_level(self->right, current_level + 1);
}

static void triple_ast_node_destroy(struct i_ast_node *node) {
  struct triple_ast_node *self = (struct triple_ast_node *)node;

  ast_node_destroy(self->left);
  ast_node_destroy(self->mid);
  ast_node_destroy(self->right);
  free(self);
}

static struct triple_ast_node *triple_ast_node_new(struct i_ast_node *left,
                                                   struct i_ast_node *mid,
                                                   struct i_ast_node *right,
                                                   str_t name) {
  struct triple_ast_node *self = malloc(sizeof(struct triple_ast_node));

  self->left = left;
  self->mid = mid;
  self->right = right;
  self->parent.name = name;

  self->parent.equals_impl = triple_ast_node_equals;
  self->parent.print_at_level_impl = triple_ast_node_print;
  self->parent.destroy_impl = triple_ast_node_destroy;
  return self;
}

// complex_ast_node
static bool complex_ast_node_equals(struct i_ast_node *node1,
                                    struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct complex_ast_node *self = (struct complex_ast_node *)node1;
  struct complex_ast_node *other = (struct complex_ast_node *)node2;

  struct queue_iterator *it_self = queue_get_entries(self->children);
  struct queue_iterator *it_other = queue_get_entries(other->children);

  while (queue_iterator_has_next(it_self) &&
         queue_iterator_has_next(it_other)) {
    struct i_ast_node **child_self = NULL;
    queue_iterator_next(it_self, (void **)&child_self);

    struct i_ast_node **child_other = NULL;
    queue_iterator_next(it_other, (void **)&child_other);

    if (!ast_node_equals(*child_self, *child_other)) {
      queue_iterator_destroy(it_self);
      queue_iterator_destroy(it_other);
      return false;
    }
  }
  if (queue_iterator_has_next(it_self) || queue_iterator_has_next(it_other))
    return false;
  return true;
}

static void complex_ast_node_print(struct i_ast_node *node,
                                   size_t current_level) {
  struct complex_ast_node *self = (struct complex_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->parent.name._data);

  struct queue_iterator *it = queue_get_entries(self->children);
  while (queue_iterator_has_next(it)) {
    struct i_ast_node **child_prt = NULL;
    queue_iterator_next(it, (void **)&child_prt);

    ast_node_print_at_level(*child_prt, current_level + 1);
  }
  queue_iterator_destroy(it);
}

static void complex_ast_node_destroy_child_entry(void *child_entry) {
  ast_node_destroy(*(struct i_ast_node **)child_entry);
}

static void complex_ast_node_destroy(struct i_ast_node *node) {
  struct complex_ast_node *self = (struct complex_ast_node *)node;

  queue_destroy(self->children);
  free(self);
}

static struct complex_ast_node *complex_ast_node_new(str_t name) {
  struct complex_ast_node *self = malloc(sizeof(struct complex_ast_node));

  self->parent.name = name;
  self->children = queue_new();
  queue_ctor(self->children, sizeof(void *),
             complex_ast_node_destroy_child_entry);

  self->parent.equals_impl = complex_ast_node_equals;
  self->parent.print_at_level_impl = complex_ast_node_print;
  self->parent.destroy_impl = complex_ast_node_destroy;
  return self;
}

static void complex_ast_node_add(struct complex_ast_node *self,
                                 struct i_ast_node *child) {
  queue_push_back(self->children, &child);
}

// actual nodes
struct i_ast_node *ast_node_stmt_new(struct i_ast_node *stmt) {
  return (struct i_ast_node *)simple_ast_node_new(stmt, STR_OF("STATEMENT"));
}

struct i_ast_node *ast_node_select_stmt_new(struct i_ast_node *from,
                                            struct i_ast_node *joins,
                                            struct i_ast_node *where) {
  return (struct i_ast_node *)triple_ast_node_new(from, joins, where,
                                                  STR_OF("SELECT STATEMENT"));
}

struct i_ast_node *ast_node_from_new(struct i_ast_node *table_id) {
  return (struct i_ast_node *)simple_ast_node_new(table_id, STR_OF("FROM"));
}

struct i_ast_node *ast_node_joins_new() {
  return (struct i_ast_node *)complex_ast_node_new(STR_OF("JOINS LIST"));
}

struct ast_node_joins *ast_node_joins_add(struct ast_node_joins *self,
                                          struct i_ast_node *join) {
  complex_ast_node_add(&self->node, join);
  return self;
}

struct i_ast_node *ast_node_join_on_new(struct i_ast_node *table_id,
                                        struct i_ast_node *predicate) {
  return (struct i_ast_node *)double_ast_node_new(table_id, predicate,
                                                  STR_OF("JOIN ON"));
}

struct i_ast_node *ast_node_where_new(struct i_ast_node *predicate) {
  return (struct i_ast_node *)simple_ast_node_new(predicate, STR_OF("WHERE"));
}

struct i_ast_node *ast_node_predicate_new(struct i_ast_node *expression) {
  return (struct i_ast_node *)simple_ast_node_new(expression,
                                                  STR_OF("PREDICATE"));
}

struct i_ast_node *ast_node_operator_expr_new(struct i_ast_node *left_expr,
                                              struct i_ast_node *oper,
                                              struct i_ast_node *right_expr) {
  return (struct i_ast_node *)triple_ast_node_new(left_expr, oper, right_expr,
                                                  STR_OF("OPERATOR EXPR"));
}

struct i_ast_node *ast_node_literal_expr_new(struct i_ast_node *value) {
  return (struct i_ast_node *)simple_ast_node_new(value, STR_OF("LITERAL"));
}

struct i_ast_node *ast_node_member_expr_new(struct i_ast_node *member_id) {
  return (struct i_ast_node *)simple_ast_node_new(member_id,
                                                  STR_OF("MEMBER EXPR"));
}

struct i_ast_node *ast_node_member_id_new(struct i_ast_node *table_id,
                                          struct i_ast_node *column_id) {
  return (struct i_ast_node *)double_ast_node_new(table_id, column_id,
                                                  STR_OF("MEMBER ID"));
}

struct i_ast_node *ast_node_table_id_new(struct i_ast_node *identifier) {
  return (struct i_ast_node *)simple_ast_node_new(identifier,
                                                  STR_OF("TABLE ID"));
}

struct i_ast_node *ast_node_column_id_new(struct i_ast_node *identifier) {
  return (struct i_ast_node *)simple_ast_node_new(identifier,
                                                  STR_OF("COLUMN ID"));
}

// comparison_oper
static bool ast_node_comparison_oper_equals(struct i_ast_node *node1,
                                            struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_comparison_oper *self =
      (struct ast_node_comparison_oper *)node1;
  struct ast_node_comparison_oper *other =
      (struct ast_node_comparison_oper *)node2;

  return self->oper == other->oper;
}

static void ast_node_comparison_oper_print(struct i_ast_node *node,
                                           size_t current_level) {
  struct ast_node_comparison_oper *self =
      (struct ast_node_comparison_oper *)node;

  PRINT_INDENT(current_level)
  char *value = NULL;
  switch (self->oper) {
  case EQ:
    value = "EQ";
    break;
  case NEQ:
    value = "NEQ";
    break;
  case LESS:
    value = "LESS";
    break;
  case LEQ:
    value = "LEQ";
    break;
  case GREATER:
    value = "GREATER";
    break;
  case GEQ:
    value = "GEQ";
    break;
  }

  printf("COMPARISON OPER: %s\n", value);
}

struct i_ast_node *ast_node_comparison_oper_new(comparison_operator_t oper) {
  struct ast_node_comparison_oper *self =
      malloc(sizeof(struct ast_node_comparison_oper));

  self->oper = oper;

  self->parent.name = STR_OF("COMPARISON OPER");
  self->parent.equals_impl = ast_node_comparison_oper_equals;
  self->parent.print_at_level_impl = ast_node_comparison_oper_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// arithmetic_oper
static bool ast_node_arithmetic_oper_equals(struct i_ast_node *node1,
                                            struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_arithmetic_oper *self =
      (struct ast_node_arithmetic_oper *)node1;
  struct ast_node_arithmetic_oper *other =
      (struct ast_node_arithmetic_oper *)node2;

  return self->oper == other->oper;
}

static void ast_node_arithmetic_oper_print(struct i_ast_node *node,
                                           size_t current_level) {
  struct ast_node_arithmetic_oper *self =
      (struct ast_node_arithmetic_oper *)node;

  PRINT_INDENT(current_level)
  char *value = NULL;
  switch (self->oper) {
  case PLUS:
    value = "PLUS";
    break;
  case MINUS:
    value = "MINUS";
    break;
  case MUL:
    value = "MUL";
    break;
  case DIV:
    value = "DIV";
    break;
  }

  printf("ARITHMETIC OPER: %s\n", value);
}

struct i_ast_node *ast_node_arithmetic_oper_new(arithmetic_operator_t oper) {
  struct ast_node_arithmetic_oper *self =
      malloc(sizeof(struct ast_node_arithmetic_oper));

  self->oper = oper;

  self->parent.name = STR_OF("ARITHMETIC OPER");
  self->parent.equals_impl = ast_node_arithmetic_oper_equals;
  self->parent.print_at_level_impl = ast_node_arithmetic_oper_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// logical_oper
static bool ast_node_logical_oper_equals(struct i_ast_node *node1,
                                         struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_logical_oper *self = (struct ast_node_logical_oper *)node1;
  struct ast_node_logical_oper *other = (struct ast_node_logical_oper *)node2;

  return self->oper == other->oper;
}

static void ast_node_logical_oper_print(struct i_ast_node *node,
                                        size_t current_level) {
  struct ast_node_logical_oper *self = (struct ast_node_logical_oper *)node;

  PRINT_INDENT(current_level)
  char *value = NULL;
  switch (self->oper) {

  case AND:
    value = "AND";
    break;
  case OR:
    value = "OR";
    break;
  case NOT:
    value = "NOT";
    break;
  }

  printf("LOGICAL OPER: %s\n", value);
}

struct i_ast_node *ast_node_logical_oper_new(logical_operator_t oper) {
  struct ast_node_logical_oper *self =
      malloc(sizeof(struct ast_node_logical_oper));

  self->oper = oper;

  self->parent.name = STR_OF("LOGICAL OPER");
  self->parent.equals_impl = ast_node_logical_oper_equals;
  self->parent.print_at_level_impl = ast_node_logical_oper_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// int
static bool ast_node_int_equals(struct i_ast_node *node1,
                                struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_int *self = (struct ast_node_int *)node1;
  struct ast_node_int *other = (struct ast_node_int *)node2;

  return self->value == other->value;
}

static void ast_node_int_print(struct i_ast_node *node, size_t current_level) {
  struct ast_node_int *self = (struct ast_node_int *)node;

  PRINT_INDENT(current_level)
  printf("INT: %d\n", self->value);
}

struct i_ast_node *ast_node_int_new(int32_t value) {
  struct ast_node_int *self = malloc(sizeof(struct ast_node_int));

  self->value = value;

  self->parent.name = STR_OF("INT");
  self->parent.equals_impl = ast_node_int_equals;
  self->parent.print_at_level_impl = ast_node_int_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// float
static bool ast_node_float_equals(struct i_ast_node *node1,
                                  struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_float *self = (struct ast_node_float *)node1;
  struct ast_node_float *other = (struct ast_node_float *)node2;

  return self->value == other->value; // TODO think
}

static void ast_node_float_print(struct i_ast_node *node,
                                 size_t current_level) {
  struct ast_node_float *self = (struct ast_node_float *)node;

  PRINT_INDENT(current_level)
  printf("FLOAT: %f\n", self->value);
}

struct i_ast_node *ast_node_float_new(float value) {
  struct ast_node_float *self = malloc(sizeof(struct ast_node_float));

  self->value = value;

  self->parent.name = STR_OF("FLOAT");
  self->parent.equals_impl = ast_node_float_equals;
  self->parent.print_at_level_impl = ast_node_float_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// bool
static bool ast_node_bool_equals(struct i_ast_node *node1,
                                 struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_bool *self = (struct ast_node_bool *)node1;
  struct ast_node_bool *other = (struct ast_node_bool *)node2;

  return self->value == other->value;
}

static void ast_node_bool_print(struct i_ast_node *node, size_t current_level) {
  struct ast_node_bool *self = (struct ast_node_bool *)node;

  PRINT_INDENT(current_level)
  printf("BOOL: %s\n", self->value ? "TRUE" : "FALSE");
}

struct i_ast_node *ast_node_bool_new(bool value) {
  struct ast_node_bool *self = malloc(sizeof(struct ast_node_bool));

  self->value = value;

  self->parent.name = STR_OF("BOOL");
  self->parent.equals_impl = ast_node_bool_equals;
  self->parent.print_at_level_impl = ast_node_bool_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// string
static bool ast_node_string_equals(struct i_ast_node *node1,
                                   struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_string *self = (struct ast_node_string *)node1;
  struct ast_node_string *other = (struct ast_node_string *)node2;

  return str_eq(string_as_str(self->value), string_as_str(other->value));
}

static void ast_node_string_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct ast_node_string *self = (struct ast_node_string *)node;

  PRINT_INDENT(current_level)
  printf("STRING: %s\n", self->value._data);
}

static void ast_node_string_destroy(struct i_ast_node *node) {
  struct ast_node_string *self = (struct ast_node_string *)node;

  string_destroy(self->value);
  free(self);
}

struct i_ast_node *ast_node_string_new(char *value) {
  struct ast_node_string *self = malloc(sizeof(struct ast_node_string));

  self->value = string_from(value);

  self->parent.name = STR_OF("STRING");
  self->parent.equals_impl = ast_node_string_equals;
  self->parent.print_at_level_impl = ast_node_string_print;
  self->parent.destroy_impl = ast_node_string_destroy;
  return (struct i_ast_node *)self;
}

// id
static bool ast_node_id_equals(struct i_ast_node *node1,
                               struct i_ast_node *node2) {
  if (!common_ast_node_equals(node1, node2))
    return false;

  struct ast_node_id *self = (struct ast_node_id *)node1;
  struct ast_node_id *other = (struct ast_node_id *)node2;

  return str_eq(string_as_str(self->id), string_as_str(other->id));
}

static void ast_node_id_print(struct i_ast_node *node, size_t current_level) {
  struct ast_node_id *self = (struct ast_node_id *)node;

  PRINT_INDENT(current_level)
  printf("ID: %s\n", self->id._data);
}

static void ast_node_id_destroy(struct i_ast_node *node) {
  struct ast_node_id *self = (struct ast_node_id *)node;

  string_destroy(self->id);
  free(self);
}

struct i_ast_node *ast_node_id_new(char *value) {
  struct ast_node_id *self = malloc(sizeof(struct ast_node_id));

  self->id = string_from(value);

  self->parent.name = STR_OF("ID");
  self->parent.equals_impl = ast_node_id_equals;
  self->parent.print_at_level_impl = ast_node_id_print;
  self->parent.destroy_impl = ast_node_id_destroy;
  return (struct i_ast_node *)self;
}