//
// Created by draen on 25.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODES_H
#define LLP_LAB_PARSER_INCLUDE_PARSER_PUBLIC_AST_NODES_H

#include "ast_node_interface.h"
#include "common/public/domain/column_types.h"
#include "common/public/domain/expression_operator_types.h"
#include <stdbool.h>
#include <stdint.h>

struct ast_node_stmt;
struct i_ast_node *ast_node_stmt_new(struct i_ast_node *stmt);

struct ast_node_select_stmt;
struct i_ast_node *ast_node_select_stmt_new(struct i_ast_node *from,
                                            struct i_ast_node *joins,
                                            struct i_ast_node *where);

struct ast_node_insert_stmt;
struct i_ast_node *ast_node_insert_stmt_new(struct i_ast_node *table_id,
                                            struct i_ast_node *col_names,
                                            struct i_ast_node *values_list);

struct ast_node_update_stmt;
struct i_ast_node *ast_node_update_stmt_new(struct i_ast_node *table_id,
                                            struct i_ast_node *updates,
                                            struct i_ast_node *where);

struct ast_node_delete_stmt;
struct i_ast_node *ast_node_delete_stmt_new(struct i_ast_node *from,
                                            struct i_ast_node *where);

struct ast_node_create_table_stmt;
struct i_ast_node *
ast_node_create_table_stmt_new(struct i_ast_node *table_id,
                               struct i_ast_node *columns_defs);

struct ast_node_drop_table_stmt;
struct i_ast_node *ast_node_drop_table_stmt_new(struct i_ast_node *table_id);

struct ast_node_columns_defs;
struct i_ast_node *ast_node_columns_defs_new();
struct ast_node_columns_defs *
ast_node_columns_defs_add(struct ast_node_columns_defs *self,
                          struct i_ast_node *column_def);

struct ast_node_column_def;
struct i_ast_node *ast_node_column_def_new(struct i_ast_node *column_id,
                                           struct i_ast_node *column_type);

struct ast_node_updates;
struct i_ast_node *ast_node_updates_new();
struct ast_node_updates *ast_node_updates_add(struct ast_node_updates *self,
                                              struct i_ast_node *update);

struct ast_node_update;
struct i_ast_node *ast_node_update_new(struct i_ast_node *column_id,
                                       struct i_ast_node *expression);

struct ast_node_col_names;
struct i_ast_node *ast_node_col_names_new();
struct ast_node_col_names *
ast_node_col_names_add(struct ast_node_col_names *self,
                       struct i_ast_node *column_id);

struct ast_node_values_list;
struct i_ast_node *ast_node_values_list_new();
struct ast_node_values_list *
ast_node_values_list_add(struct ast_node_values_list *self,
                         struct i_ast_node *values);

struct ast_node_values;
struct i_ast_node *ast_node_values_new();
struct ast_node_values *ast_node_values_add(struct ast_node_values *self,
                                            struct i_ast_node *expr);

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

struct ast_node_column_type;
struct i_ast_node *ast_node_column_type_new(column_type_t type);

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
