//
// Created by draen on 28.10.23.
//

#include "test_statement.h"

int test_statement(str_t statement, struct i_ast_node *expected) {
  struct i_ast_node *tree = NULL;
  TRY(parse_string(statement, &tree));
  CATCH(error, {
    handle_error(error);
    ast_node_destroy(expected);
    return 1;
  })

  bool res = ast_node_equals(tree, expected);
  ast_node_destroy(tree);
  ast_node_destroy(expected);
  return res ? 0 : 1;
}