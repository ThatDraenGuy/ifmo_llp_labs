//
// Created by draen on 28.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_delete_stmt_new(
      ast_node_from_new(ast_node_table_id_new(ast_node_id_new("aboba"))),
      ast_node_where_new(ast_node_predicate_new(ast_node_operator_expr_new(
          ast_node_member_expr_new(ast_node_member_id_new(
              ast_node_table_id_new(ast_node_id_new("aboba")),
              ast_node_column_id_new(ast_node_id_new("a")))),
          ast_node_comparison_oper_new(EQ),
          ast_node_literal_expr_new(ast_node_float_new(3.0f)))))));
  return test_statement(STR_OF("DELETE FROM aboba WHERE aboba.a = 3.0;"),
                        expected);
}