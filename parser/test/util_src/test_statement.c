//
// Created by draen on 28.10.23.
//

#include "test_statement.h"
#include <stdio.h>

int test_statement(str_t statement, struct i_ast_node *expected) {
  struct i_ast_node *tree = NULL;
  TRY(parse_string(statement, &tree));
  CATCH(error, {
    handle_error(error);
    ast_node_destroy(expected);
    return 1;
  })

  bool res = ast_node_equals(tree, expected);
  if (!res) {
    printf("-------Expected tree: \n");
    ast_node_print(expected);
    printf("\n-------Actual tree: \n");
    ast_node_print(tree);
  }
  ast_node_destroy(tree);
  ast_node_destroy(expected);
  return res ? 0 : 1;
}

int test_error(str_t statement) {
  struct i_ast_node *tree = NULL;
  TRY(parse_string(statement, &tree));
  CATCH(error, {
    error_destroy(error);
    return 0;
  })

  ast_node_destroy(tree);
  return 1;
}