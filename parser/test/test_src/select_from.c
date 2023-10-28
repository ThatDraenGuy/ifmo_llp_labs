//
// Created by draen on 20.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_select_stmt_new(
      ast_node_from_new(ast_node_table_id_new(ast_node_id_new("aboba"))), NULL,
      NULL));
  return test_statement(STR_OF("SELECT FROM aboba;"), expected);
}