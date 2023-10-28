//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODES_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODES_H

#include "ast_node_interface.h"
#include "common/public/domain/expression_operator_types.h"
#include <stdbool.h>
#include <stdint.h>

struct ast_node_stmt;
struct i_ast_node *ast_node_stmt_new(struct i_ast_node *stmt);

struct ast_node_select_stmt;
struct i_ast_node *ast_node_select_stmt_new(struct i_ast_node *from,
                                            struct i_ast_node *joins,
                                            struct i_ast_node *where);

struct ast_node_from;
struct i_ast_node *ast_node_from_new(struct i_ast_node *table_id);

struct ast_node_joins;
struct i_ast_node *ast_node_joins_new();
struct ast_node_joins *ast_node_joins_add(struct ast_node_joins *self,
                                          struct i_ast_node *join);

struct ast_node_join_on;
struct i_ast_node *ast_node_join_on_new(struct i_ast_node *table_id,
                                        struct i_ast_node *predicate);

struct ast_node_where;
struct i_ast_node *ast_node_where_new(struct i_ast_node *predicate);

struct ast_node_predicate;
struct i_ast_node *ast_node_predicate_new(struct i_ast_node *expression);

struct ast_node_operator_expr;
struct i_ast_node *ast_node_operator_expr_new(struct i_ast_node *left_expr,
                                              struct i_ast_node *oper,
                                              struct i_ast_node *right_expr);

struct ast_node_comparison_oper;
struct i_ast_node *ast_node_comparison_oper_new(comparison_operator_t oper);

struct ast_node_arithmetic_oper;
struct i_ast_node *ast_node_arithmetic_oper_new(arithmetic_operator_t oper);

struct ast_node_logical_oper;
struct i_ast_node *ast_node_logical_oper_new(logical_operator_t oper);

struct ast_node_literal_expr;
struct i_ast_node *ast_node_literal_expr_new(struct i_ast_node *value);

struct ast_node_member_expr;
struct i_ast_node *ast_node_member_expr_new(struct i_ast_node *member_id);

struct ast_node_member_id;
struct i_ast_node *ast_node_member_id_new(struct i_ast_node *table_id,
                                          struct i_ast_node *column_id);

struct ast_node_table_id;
struct i_ast_node *ast_node_table_id_new(struct i_ast_node *identifier);

struct ast_node_column_id;
struct i_ast_node *ast_node_column_id_new(struct i_ast_node *identifier);

struct ast_node_int;
struct i_ast_node *ast_node_int_new(int32_t value);

struct ast_node_float;
struct i_ast_node *ast_node_float_new(float value);

struct ast_node_bool;
struct i_ast_node *ast_node_bool_new(bool value);

struct ast_node_string;
struct i_ast_node *ast_node_string_new(char *value);

struct ast_node_id;
struct i_ast_node *ast_node_id_new(char *value);

#endif // LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODES_H
