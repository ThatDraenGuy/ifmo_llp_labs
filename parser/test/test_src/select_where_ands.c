//
// Created by draen on 20.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_select_stmt_new(
      ast_node_from_new(ast_node_table_id_new(ast_node_id_new("amogus"))), NULL,
      ast_node_where_new(ast_node_predicate_new(ast_node_operator_expr_new(
          ast_node_operator_expr_new(
              ast_node_operator_expr_new(
                  ast_node_member_expr_new(ast_node_member_id_new(
                      ast_node_table_id_new(ast_node_id_new("amogus")),
                      ast_node_column_id_new(ast_node_id_new("imposter")))),
                  ast_node_comparison_oper_new(EQ),
                  ast_node_literal_expr_new(ast_node_string_new("\"SUS\""))),
              ast_node_logical_oper_new(AND),
              ast_node_operator_expr_new(
                  ast_node_member_expr_new(ast_node_member_id_new(
                      ast_node_table_id_new(ast_node_id_new("amogus")),
                      ast_node_column_id_new(ast_node_id_new("status")))),
                  ast_node_comparison_oper_new(LESS),
                  ast_node_literal_expr_new(ast_node_int_new(52)))),
          ast_node_logical_oper_new(AND),
          ast_node_operator_expr_new(
              ast_node_member_expr_new(ast_node_member_id_new(
                  ast_node_table_id_new(ast_node_id_new("amogus")),
                  ast_node_column_id_new(ast_node_id_new("booblean")))),
              ast_node_comparison_oper_new(NEQ),
              ast_node_literal_expr_new(ast_node_bool_new(true))))))));
  return test_statement(
      STR_OF("SELECT FROM amogus WHERE amogus.imposter = \"SUS\" AND "
             "amogus.status < 52 AND amogus.booblean != TRUE;"),
      expected);
}