//
// Created by draen on 26.10.23.
//

#include "parser/private/ast_nodes.h"
#include <malloc.h>

#define PRINT_INDENT(Level)                                                    \
  for (size_t i = 0; i < Level; i++) {                                         \
    printf("\t");                                                              \
  }

static void print_placeholder(struct i_ast_node *node, size_t current_level) {}

static void common_ast_node_destroy(struct i_ast_node *node) { free(node); }

// simple_ast_node
static void simple_node_print(struct i_ast_node *node, size_t current_level) {
  struct simple_ast_node *self = (struct simple_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->name._data);
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
  self->name = name;

  self->parent.print_at_level_impl = simple_node_print;
  self->parent.destroy_impl = simple_ast_node_destroy;
  return self;
}

// double_ast_node
static void double_ast_node_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct double_ast_node *self = (struct double_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->name._data);

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
  self->name = name;

  self->parent.print_at_level_impl = double_ast_node_print;
  self->parent.destroy_impl = double_ast_node_destroy;
  return self;
}

// triple_ast_node
static void triple_ast_node_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct triple_ast_node *self = (struct triple_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->name._data);

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
  self->name = name;

  self->parent.print_at_level_impl = triple_ast_node_print;
  self->parent.destroy_impl = triple_ast_node_destroy;
  return self;
}

// complex_ast_node
static void complex_ast_node_print(struct i_ast_node *node,
                                   size_t current_level) {
  struct complex_ast_node *self = (struct complex_ast_node *)node;

  PRINT_INDENT(current_level)
  printf("%s:\n", self->name._data);

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

  self->name = name;
  self->children = queue_new();
  queue_ctor(self->children, sizeof(void *),
             complex_ast_node_destroy_child_entry);

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

void ast_node_joins_add(struct ast_node_joins *self, struct i_ast_node *join) {
  complex_ast_node_add(&self->node, join);
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

struct i_ast_node *ast_node_expression_oper_new(struct i_ast_node *oper) {
  return (struct i_ast_node *)simple_ast_node_new(oper, STR_OF("OPER"));
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

  self->parent.print_at_level_impl = ast_node_comparison_oper_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// arithmetic_oper
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

  self->parent.print_at_level_impl = ast_node_arithmetic_oper_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// int
static void ast_node_int_print(struct i_ast_node *node, size_t current_level) {
  struct ast_node_int *self = (struct ast_node_int *)node;

  PRINT_INDENT(current_level)
  printf("INT: %d", self->value);
}

struct i_ast_node *ast_node_int_new(int32_t value) {
  struct ast_node_int *self = malloc(sizeof(struct ast_node_int));

  self->value = value;

  self->parent.print_at_level_impl = ast_node_int_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// float
static void ast_node_float_print(struct i_ast_node *node,
                                 size_t current_level) {
  struct ast_node_float *self = (struct ast_node_float *)node;

  PRINT_INDENT(current_level)
  printf("FLOAT: %f", self->value);
}

struct i_ast_node *ast_node_float_new(float value) {
  struct ast_node_float *self = malloc(sizeof(struct ast_node_float));

  self->value = value;

  self->parent.print_at_level_impl = ast_node_float_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// bool
static void ast_node_bool_print(struct i_ast_node *node, size_t current_level) {
  struct ast_node_bool *self = (struct ast_node_bool *)node;

  PRINT_INDENT(current_level)
  printf("BOOL: %s", self->value ? "TRUE" : "FALSE");
}

struct i_ast_node *ast_node_bool_new(bool value) {
  struct ast_node_bool *self = malloc(sizeof(struct ast_node_bool));

  self->value = value;

  self->parent.print_at_level_impl = ast_node_bool_print;
  self->parent.destroy_impl = common_ast_node_destroy;

  return (struct i_ast_node *)self;
}

// string
static void ast_node_string_print(struct i_ast_node *node,
                                  size_t current_level) {
  struct ast_node_string *self = (struct ast_node_string *)node;

  PRINT_INDENT(current_level)
  printf("STRING: %s", self->value._data);
}

static void ast_node_string_destroy(struct i_ast_node *node) {
  struct ast_node_string *self = (struct ast_node_string *)node;

  string_destroy(self->value);
  free(self);
}

struct i_ast_node *ast_node_string_new(char *value) {
  struct ast_node_string *self = malloc(sizeof(struct ast_node_string));

  self->value = string_from(value);

  self->parent.print_at_level_impl = ast_node_string_print;
  self->parent.destroy_impl = ast_node_string_destroy;
  return (struct i_ast_node *)self;
}

// id
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

  self->parent.print_at_level_impl = ast_node_id_print;
  self->parent.destroy_impl = ast_node_id_destroy;
  return (struct i_ast_node *)self;
}