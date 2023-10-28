//
// Created by draen on 28.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_update_stmt_new(
      ast_node_table_id_new(ast_node_id_new("aboba")),
      (struct i_ast_node *)ast_node_updates_add(
          (struct ast_node_updates *)ast_node_updates_new(),
          ast_node_update_new(
              ast_node_column_id_new(ast_node_id_new("imposter")),
              ast_node_literal_expr_new(ast_node_string_new("\"SUS\"")))),
      ast_node_where_new(ast_node_predicate_new(ast_node_operator_expr_new(
          ast_node_member_expr_new(ast_node_member_id_new(
              ast_node_table_id_new(ast_node_id_new("aboba")),
              ast_node_column_id_new(ast_node_id_new("color")))),
          ast_node_comparison_oper_new(EQ),
          ast_node_literal_expr_new(ast_node_string_new("\"RED\"")))))));

  return test_statement(
      STR_OF(
          "UPDATE aboba SET imposter = \"SUS\" WHERE aboba.color = \"RED\";"),
      expected);
}