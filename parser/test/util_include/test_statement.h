//
// Created by draen on 28.10.23.
//

#ifndef LLP_LAB_PARSER_TEST_UTIL_INCLUDE_TEST_STATEMENT_H
#define LLP_LAB_PARSER_TEST_UTIL_INCLUDE_TEST_STATEMENT_H

#include "common/public/util/string.h"
#include "parser/prelude.h"

int test_statement(str_t statement, struct i_ast_node *expected);
int test_error(str_t statement);

#endif // LLP_LAB_PARSER_TEST_UTIL_INCLUDE_TEST_STATEMENT_H
