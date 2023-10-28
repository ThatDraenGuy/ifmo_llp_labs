//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODE_INTERFACE_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODE_INTERFACE_H

#include "common/public/util/string.h"
#include "parser/public/ast_node_interface.h"
#include <stddef.h>

struct i_ast_node {
  str_t name;
  bool (*equals_impl)(struct i_ast_node *self, struct i_ast_node *other);
  void (*print_at_level_impl)(struct i_ast_node *self, size_t current_level);
  void (*destroy_impl)(struct i_ast_node *self);
};

void ast_node_print_at_level(struct i_ast_node *node, size_t level);

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_AST_NODE_INTERFACE_H
