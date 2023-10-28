//
// Created by draen on 20.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_select_stmt_new(
      ast_node_from_new(ast_node_table_id_new(ast_node_id_new("aboba"))),
      (struct i_ast_node *)ast_node_joins_add(
          ast_node_joins_add(
              (struct ast_node_joins *)ast_node_joins_new(),
              ast_node_join_on_new(
                  ast_node_table_id_new(ast_node_id_new("amogus")),
                  ast_node_predicate_new(ast_node_operator_expr_new(
                      ast_node_member_expr_new(ast_node_member_id_new(
                          ast_node_table_id_new(ast_node_id_new("aboba")),
                          ast_node_column_id_new(ast_node_id_new("imposter")))),
                      ast_node_comparison_oper_new(EQ),
                      ast_node_member_expr_new(ast_node_member_id_new(
                          ast_node_table_id_new(ast_node_id_new("amogus")),
                          ast_node_column_id_new(
                              ast_node_id_new("crewmate")))))))),
          ast_node_join_on_new(
              ast_node_table_id_new(ast_node_id_new("stuff")),
              ast_node_predicate_new(ast_node_operator_expr_new(
                  ast_node_member_expr_new(ast_node_member_id_new(
                      ast_node_table_id_new(ast_node_id_new("amogus")),
                      ast_node_column_id_new(ast_node_id_new("part")))),
                  ast_node_comparison_oper_new(EQ),
                  ast_node_member_expr_new(ast_node_member_id_new(
                      ast_node_table_id_new(ast_node_id_new("stuff")),
                      ast_node_column_id_new(ast_node_id_new("field")))))))),
      ast_node_where_new(ast_node_predicate_new(ast_node_operator_expr_new(
          ast_node_member_expr_new(ast_node_member_id_new(
              ast_node_table_id_new(ast_node_id_new("amogus")),
              ast_node_column_id_new(ast_node_id_new("status")))),
          ast_node_comparison_oper_new(GEQ),
          ast_node_literal_expr_new(ast_node_int_new(52)))))));

  return test_statement(
      STR_OF(
          "SELECT FROM aboba JOIN amogus ON aboba.imposter = amogus.crewmate "
          "JOIN stuff ON amogus.part = stuff.field WHERE amogus.status >= 52;"),
      expected);
}