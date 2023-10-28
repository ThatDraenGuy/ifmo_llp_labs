//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H

struct i_ast_node;

void ast_node_print(struct i_ast_node *self);
void ast_node_destroy(struct i_ast_node *self);

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODE_INTERFACE_H
