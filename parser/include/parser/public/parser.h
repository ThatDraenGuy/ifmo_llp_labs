//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_PARSER_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_PARSER_H

#include "common/public/util/result.h"
#include "parser/public/ast_node_interface.h"

result_t parse_stdin(struct i_ast_node **result);
result_t parse_string(str_t str, struct i_ast_node **result);

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_PARSER_H
