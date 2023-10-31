//
// Created by draen on 20.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_drop_table_stmt_new(
      ast_node_table_id_new(ast_node_id_new("aboba"))));
  return test_statement(STR_OF("DROP TABLE aboba;"), expected);
}