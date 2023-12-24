//
// Created by draen on 20.12.23.
//
#include "common/public/error/error.h"
#include "parser/private/ast_node_interface.h"
#include "parser/private/ast_nodes.h"
#include "transfer/public/service_types.h"

#define CHECK_TYPE(Node, Type)                                                 \
  if (!str_eq(Node->name, STR_OF(Type))) {                                     \
    THROW(error_self(STR_OF(Type)));                                           \
  }

static struct error *error_self(str_t error_type) {
  return error_new(STR_OF("AST_MAP"), error_type, (error_code_t){0},
                   STR_OF("MAP ERR"));
}

static result_t map_table_id(struct i_ast_node *node, str_t *result) {
  CHECK_TYPE(node, "TABLE ID")

  struct ast_node_table_id *table_id = (struct ast_node_table_id *)node;
  struct ast_node_id *id = (struct ast_node_id *)table_id->node.child;

  *result = string_as_str(id->id);

  OK;
}

static result_t map_column_id(struct i_ast_node *node, str_t *result) {
  CHECK_TYPE(node, "COLUMN ID")

  struct ast_node_column_id *column_id = (struct ast_node_column_id *)node;
  struct ast_node_id *id = (struct ast_node_id *)column_id->node.child;

  *result = string_as_str(id->id);

  OK;
}

static result_t map_literal(struct i_ast_node *node, ColumnValue **result_value,
                            ColumnType *result_type) {

  if (str_eq(node->name, STR_OF("INT"))) {
    struct ast_node_int *actual_node = (struct ast_node_int *)node;
    *result_value =
        g_object_new(TYPE_COLUMN_VALUE, "i32_val", actual_node->value, NULL);
    *result_type = COLUMN_TYPE_COLUMN_TYPE_INT32;
  } else if (str_eq(node->name, STR_OF("FLOAT"))) {
    struct ast_node_float *actual_node = (struct ast_node_float *)node;
    *result_value =
        g_object_new(TYPE_COLUMN_VALUE, "float_val", actual_node->value, NULL);
    *result_type = COLUMN_TYPE_COLUMN_TYPE_FLOAT;
  } else if (str_eq(node->name, STR_OF("BOOL"))) {
    struct ast_node_bool *actual_node = (struct ast_node_bool *)node;
    *result_value =
        g_object_new(TYPE_COLUMN_VALUE, "bool_val", actual_node->value, NULL);
    *result_type = COLUMN_TYPE_COLUMN_TYPE_BOOL;
  } else if (str_eq(node->name, STR_OF("STRING"))) {
    struct ast_node_string *actual_node = (struct ast_node_string *)node;
    *result_value =
        g_object_new(TYPE_COLUMN_VALUE, "string_val", actual_node->value, NULL);
    *result_type = COLUMN_TYPE_COLUMN_TYPE_STRING;
  } else {
    THROW(error_self(STR_OF("No literal")));
  }

  OK;
}

static result_t map_column_type(struct i_ast_node *node, ColumnType *result) {
  CHECK_TYPE(node, "COLUMN TYPE")

  struct ast_node_column_type *type = (struct ast_node_column_type *)node;

  switch (type->type) {
  case COLUMN_TYPE_INT32:
    *result = COLUMN_TYPE_COLUMN_TYPE_INT32;
    break;
  case COLUMN_TYPE_UINT64:
    *result = COLUMN_TYPE_COLUMN_TYPE_UINT64;
    break;
  case COLUMN_TYPE_FLOAT:
    *result = COLUMN_TYPE_COLUMN_TYPE_FLOAT;
    break;
  case COLUMN_TYPE_STRING:
    *result = COLUMN_TYPE_COLUMN_TYPE_STRING;
    break;
  case COLUMN_TYPE_BOOL:
    *result = COLUMN_TYPE_COLUMN_TYPE_BOOL;
    break;
  }
  OK;
}

static result_t map_expression(struct i_ast_node *node, Expression **result) {
  // TODO
}

static result_t map_predicate(struct i_ast_node *node, Predicate **result) {
  CHECK_TYPE(node, "PREDICATE")

  struct ast_node_predicate *predicate = (struct ast_node_predicate *)node;

  Expression *g_expr;
  TRY(map_expression(predicate->node.child, &g_expr));
  CATCH(error, THROW(error))

  *result = g_object_new(TYPE_PREDICATE, "expression", g_expr, NULL);
  OK;
}

static result_t map_column_defs(struct i_ast_node *node, GPtrArray **result) {
  CHECK_TYPE(node, "COLUMN DEFS")

  struct ast_node_columns_defs *defs = (struct ast_node_columns_defs *)node;

  *result = g_ptr_array_new_with_free_func(g_object_unref);
  struct queue_iterator *it = queue_get_entries(defs->node.children);
  while (queue_iterator_has_next(it)) {
    ColumnSchema *schema;

    struct ast_node_column_def **def = NULL;
    queue_iterator_next(it, (void **)&def);

    str_t column_name = STR_NULL;
    TRY(map_column_id((*def)->node.left, &column_name));
    CATCH(error, {
      g_object_unref(*result);
      queue_iterator_destroy(it);
      THROW(error);
    })

    ColumnType column_type;
    TRY(map_column_type((*def)->node.right, &column_type));
    CATCH(error, {
      g_ptr_array_unref(*result);
      queue_iterator_destroy(it);
      THROW(error);
    })

    schema = g_object_new(TYPE_COLUMN_SCHEMA, "columnName",
                          str_get_c_string(column_name), "columnType",
                          column_type, NULL);

    g_ptr_array_add(*result, schema);
  }

  queue_iterator_destroy(it);
  OK;
}

static result_t map_create_table_stmt(struct i_ast_node *node,
                                      CreateTableStmt **result) {
  CHECK_TYPE(node, "CREATE TABLE STATEMENT")

  struct ast_node_create_table_stmt *create =
      (struct ast_node_create_table_stmt *)node;

  str_t table_id = STR_NULL;
  TRY(map_table_id(create->node.left, &table_id));
  CATCH(error, THROW(error))

  GPtrArray *column_defs;
  TRY(map_column_defs(create->node.right, &column_defs));
  CATCH(error, THROW(error))

  TableSchema *schema;
  schema =
      g_object_new(TYPE_TABLE_SCHEMA, "tableName", str_get_c_string(table_id),
                   "columns", column_defs, NULL);

  *result = g_object_new(TYPE_CREATE_TABLE_STMT, "schema", schema, NULL);

  OK;
}

static result_t map_drop_table_stmt(struct i_ast_node *node,
                                    DropTableStmt **result) {
  CHECK_TYPE(node, "DROP TABLE STATEMENT")

  struct ast_node_drop_table_stmt *drop =
      (struct ast_node_drop_table_stmt *)node;

  str_t table_id = STR_NULL;
  TRY(map_table_id(drop->node.child, &table_id));
  CATCH(error, THROW(error))

  *result = g_object_new(TYPE_DROP_TABLE_STMT, "tableName",
                         str_get_c_string(table_id), NULL);

  OK;
}

static result_t map_select_stmt(struct i_ast_node *node, QueryStmt **result) {
  CHECK_TYPE(node, "SELECT STATEMENT")

  struct ast_node_select_stmt *select = (struct ast_node_select_stmt *)node;
  struct ast_node_from *from = (struct ast_node_from *)select->node.left;

  str_t table_id = STR_NULL;
  TRY(map_table_id(from->node.child, &table_id));
  CATCH(error, THROW(error))

  struct ast_node_joins *joins = (struct ast_node_joins *)select->node.mid;

  GPtrArray *g_joins = g_ptr_array_new_with_free_func(g_object_unref);
  struct queue_iterator *it = queue_get_entries(joins->node.children);
  while (queue_iterator_has_next(it)) {
    Join *g_join;

    struct ast_node_join_on **join = NULL;
    queue_iterator_next(it, (void **)&join);

    str_t join_table = STR_NULL;
    TRY(map_table_id((*join)->node.left, &join_table));
    CATCH(error, {
      g_ptr_array_unref(g_joins);
      queue_iterator_destroy(it);
      THROW(error);
    })

    Predicate *join_on;
    TRY(map_predicate((*join)->node.right, &join_on));
    CATCH(error, {
      g_ptr_array_unref(g_joins);
      queue_iterator_destroy(it);
      THROW(error);
    })

    g_join = g_object_new(TYPE_JOIN, "what", str_get_c_string(join_table), "on",
                          join_on, NULL);

    g_ptr_array_add(g_joins, g_join);
  }
  queue_iterator_destroy(it);

  struct ast_node_where *where = (struct ast_node_where *)select->node.right;
  Predicate *g_where;
  TRY(map_predicate(where->node.child, &g_where));
  CATCH(error, {
    g_ptr_array_unref(g_joins);
    THROW(error);
  })

  *result =
      g_object_new(TYPE_QUERY_STMT, "fromTable", str_get_c_string(table_id),
                   "where", g_where, "joins", g_joins, NULL);

  OK;
}

static result_t map_insert_stmt(struct i_ast_node *node, InsertStmt **result) {
  CHECK_TYPE(node, "INSERT STATEMENT")

  struct ast_node_insert_stmt *insert = (struct ast_node_insert_stmt *)node;

  str_t table_id;
  TRY(map_table_id(insert->node.left, &table_id));
  CATCH(error, THROW(error))

  struct ast_node_col_names *col_names =
      (struct ast_node_col_names *)insert->node.mid;

  struct ast_node_values_list *values_list =
      (struct ast_node_values_list *)insert->node.right;

  RecordGroup *g_record_group;
  TableSchema *g_schema;
  GPtrArray *g_column_schemas = g_ptr_array_new_with_free_func(g_object_unref);

  GPtrArray *g_records = g_ptr_array_new_with_free_func(g_object_unref);

  struct queue_iterator *it = queue_get_entries(values_list->node.children);
  bool first = true;
  while (queue_iterator_has_next(it)) {
    Record *g_record;
    GPtrArray *g_values = g_ptr_array_new_with_free_func(g_object_unref);
    struct ast_node_values **values = NULL;
    queue_iterator_next(it, (void **)&values);

    struct queue_iterator *names_it =
        queue_get_entries(col_names->node.children);
    struct queue_iterator *values_it =
        queue_get_entries((*values)->node.children);
    while (queue_iterator_has_next(values_it) &&
           queue_iterator_has_next(names_it)) {
      ColumnSchema *col_schema;

      struct i_ast_node **value;
      queue_iterator_next(values_it, (void **)&value);
      struct i_ast_node **column_id;
      queue_iterator_next(names_it, (void **)&column_id);
      str_t column_name = STR_NULL;
      TRY(map_column_id(*column_id, &column_name));
      CATCH(error, {
        g_ptr_array_unref(g_records);
        g_ptr_array_unref(g_column_schemas);
        queue_iterator_destroy(it);
        queue_iterator_destroy(values_it);
        queue_iterator_destroy(names_it);
        THROW(error);
      })

      ColumnType column_type;
      ColumnValue *column_value;
      TRY(map_literal(*value, &column_value, &column_type));
      CATCH(error, {
        g_ptr_array_unref(g_records);
        g_ptr_array_unref(g_column_schemas);
        queue_iterator_destroy(it);
        queue_iterator_destroy(values_it);
        queue_iterator_destroy(names_it);
        THROW(error);
      })

      g_ptr_array_add(g_values, column_value);
      if (first) {
        col_schema = g_object_new(TYPE_COLUMN_SCHEMA, "columnName",
                                  str_get_c_string(column_name), "columnType",
                                  column_type, NULL);
        g_ptr_array_add(g_column_schemas, col_schema);
      }
    }
    queue_iterator_destroy(values_it);
    queue_iterator_destroy(names_it);

    g_record = g_object_new(TYPE_RECORD, "columnValues", g_values, NULL);
    g_ptr_array_add(g_records, g_record);
    first = false;
  }
  queue_iterator_destroy(it);

  g_schema =
      g_object_new(TYPE_TABLE_SCHEMA, "tableName", str_get_c_string(table_id),
                   "columns", g_column_schemas, NULL);
  g_record_group = g_object_new(TYPE_RECORD_GROUP, "schema", g_schema,
                                "records", g_records, NULL);
  *result = g_object_new(TYPE_INSERT_STMT, "into", str_get_c_string(table_id),
                         "records", g_record_group, NULL);
  OK;
}

static result_t map_update_stmt(struct i_ast_node *node, UpdateStmt **result) {
  CHECK_TYPE(node, "UPDATE STATEMENT")

  // TODO
}

static result_t map_delete_stmt(struct i_ast_node *node, DeleteStmt **result) {
  CHECK_TYPE(node, "DELETE STATEMENT")

  struct ast_node_delete_stmt *delete = (struct ast_node_delete_stmt *)node;

  struct ast_node_from *from = (struct ast_node_from *)delete->node.left;

  str_t table_id = STR_NULL;
  TRY(map_table_id(from->node.child, &table_id));
  CATCH(error, THROW(error))

  struct ast_node_where *where = (struct ast_node_where *)delete->node.right;
  Predicate *g_where;
  TRY(map_predicate(where->node.child, &g_where));
  CATCH(error, THROW(error))

  *result = g_object_new(TYPE_DELETE_STMT, "fromTable",
                         str_get_c_string(table_id), "where", g_where, NULL);
  OK;
}

result_t map_stmt(struct i_ast_node *node, Statement **result) {
  CHECK_TYPE(node, "STATEMENT")

  struct ast_node_stmt *stmt = (struct ast_node_stmt *)node;
  struct i_ast_node *stmt_node = stmt->node.child;

  if (str_eq(stmt_node->name, STR_OF("CREATE TABLE STATEMENT"))) {
    CreateTableStmt *create_table;
    TRY(map_create_table_stmt(stmt_node, &create_table));
    CATCH(error, THROW(error))

    *result =
        g_object_new(TYPE_STATEMENT, "createTableStmt", create_table, NULL);
  } else if (str_eq(stmt_node->name, STR_OF("DROP TABLE STATEMENT"))) {
    DropTableStmt *drop_table;
    TRY(map_drop_table_stmt(stmt_node, &drop_table));
    CATCH(error, THROW(error))

    *result = g_object_new(TYPE_STATEMENT, "dropTableStmt", drop_table, NULL);
  } else if (str_eq(stmt_node->name, STR_OF("SELECT STATEMENT"))) {
    QueryStmt *select;
    TRY(map_select_stmt(stmt_node, &select));
    CATCH(error, THROW(error))

    *result = g_object_new(TYPE_STATEMENT, "queryStmt", select, NULL);
  } else if (str_eq(stmt_node->name, STR_OF("INSERT STATEMENT"))) {
    InsertStmt *insert;
    TRY(map_insert_stmt(stmt_node, &insert));
    CATCH(error, THROW(error))

    *result = g_object_new(TYPE_STATEMENT, "insertStmt", insert, NULL);
  } else if (str_eq(stmt_node->name, STR_OF("UPDATE STATEMENT"))) {
    UpdateStmt *update;
    TRY(map_update_stmt(stmt_node, &update));
    CATCH(error, THROW(error))

    *result = g_object_new(TYPE_STATEMENT, "updateStmt", update, NULL);
  } else if (str_eq(stmt_node->name, STR_OF("DELETE STATEMENT"))) {
    DeleteStmt *select;
    TRY(map_delete_stmt(stmt_node, &select));
    CATCH(error, THROW(error))

    *result = g_object_new(TYPE_STATEMENT, "deleteStmt", select, NULL);
  } else {
    THROW(error_self(STR_OF("No statement")));
  }
  OK;
}