//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H

#include <stdbool.h>

struct i_ast_node;

bool ast_node_equals(struct i_ast_node *self, struct i_ast_node *other);
void ast_node_print(struct i_ast_node *self);
void ast_node_destroy(struct i_ast_node *self);

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H
