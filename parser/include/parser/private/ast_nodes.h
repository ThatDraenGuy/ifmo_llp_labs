//
// Created by draen on 26.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODES_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODES_H

#include "common/public/util/queue.h"
#include "common/public/util/string.h"
#include "parser/private/ast_node_interface.h"
#include "parser/public/ast_nodes.h"

struct simple_ast_node {
  struct i_ast_node parent;
  struct i_ast_node *child;
};

struct double_ast_node {
  struct i_ast_node parent;
  struct i_ast_node *left;
  struct i_ast_node *right;
};

struct triple_ast_node {
  struct i_ast_node parent;
  struct i_ast_node *left;
  struct i_ast_node *mid;
  struct i_ast_node *right;
};

struct complex_ast_node {
  struct i_ast_node parent;
  struct queue *children;
};

struct ast_node_stmt {
  struct simple_ast_node node;
};

struct ast_node_select_stmt {
  struct triple_ast_node node;
};

struct ast_node_from {
  struct simple_ast_node node;
};

struct ast_node_joins {
  struct complex_ast_node node;
};

struct ast_node_join_on {
  struct double_ast_node node;
};

struct ast_node_where {
  struct simple_ast_node node;
};

struct ast_node_predicate {
  struct simple_ast_node node;
};

struct ast_node_operator_expr {
  struct triple_ast_node node;
};

struct ast_node_comparison_oper {
  struct i_ast_node parent;
  comparison_operator_t oper;
};

struct ast_node_arithmetic_oper {
  struct i_ast_node parent;
  arithmetic_operator_t oper;
};

struct ast_node_logical_oper {
  struct i_ast_node parent;
  logical_operator_t oper;
};

struct ast_node_literal_expr {
  struct simple_ast_node node;
};

struct ast_node_member_expr {
  struct simple_ast_node node;
};

struct ast_node_member_id {
  struct double_ast_node node;
};

struct ast_node_table_id {
  struct simple_ast_node node;
};

struct ast_node_column_id {
  struct simple_ast_node node;
};

struct ast_node_int {
  struct i_ast_node parent;
  int32_t value;
};

struct ast_node_float {
  struct i_ast_node parent;
  float value;
};

struct ast_node_bool {
  struct i_ast_node parent;
  bool value;
};

struct ast_node_string {
  struct i_ast_node parent;
  string_t value;
};

struct ast_node_id {
  struct i_ast_node parent;
  string_t id;
};

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODES_H
