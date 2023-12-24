//
// Created by draen on 28.10.23.
//

#include "parser/public/ast_nodes.h"
#include "test_statement.h"

int main() {
  struct i_ast_node *expected = ast_node_stmt_new(ast_node_insert_stmt_new(
      ast_node_table_id_new(ast_node_id_new("aboba")),
      (struct i_ast_node *)ast_node_col_names_add(
          ast_node_col_names_add(
              (struct ast_node_col_names *)ast_node_col_names_new(),
              ast_node_column_id_new(ast_node_id_new("is_imposter"))),
          ast_node_column_id_new(ast_node_id_new("name"))),
      (struct i_ast_node *)ast_node_values_list_add(
          ast_node_values_list_add(
              (struct ast_node_values_list *)ast_node_values_list_new(),
              (struct i_ast_node *)ast_node_values_add(
                  ast_node_values_add(
                      (struct ast_node_values *)ast_node_values_new(),
                      ast_node_bool_new(true)),
                  ast_node_string_new("\"imposter\""))),
          (struct i_ast_node *)ast_node_values_add(
              ast_node_values_add(
                  (struct ast_node_values *)ast_node_values_new(),
                  ast_node_bool_new(false)),
              ast_node_string_new("\"crewmate\"")))));
  return test_statement(
      STR_OF(
          "INSERT INTO aboba (is_imposter, name) VALUES (TRUE, \"imposter\"), "
          "(FALSE, \"crewmate\");"),
                        expected);
}