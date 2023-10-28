//
// Created by draen on 27.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_PARSER_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_PARSER_H

#include "common/public/error/error.h"
#include "parser/public/ast_node_interface.h"
#include "parser/public/parser.h"
#include <stdbool.h>
#include <stdio.h>

struct parser_ctx {
  bool is_error;
  struct i_ast_node *root;
  struct error *error;
};

void parser_ctx_handle_error(struct parser_ctx *self, char *fmt, ...);
#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PRIVATE_PARSER_H
