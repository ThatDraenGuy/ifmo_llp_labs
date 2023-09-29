//
// Created by draen on 25.09.23.
//

#include "private/database/domain/statements.h"
#include <malloc.h>

static result_t
create_table_statement_execute(struct i_statement *create_table_statement,
                               struct table_manager *table_manager,
                               struct statement_result *statement_result) {
  struct create_table_statement *self =
      (struct create_table_statement *)create_table_statement;

  TRY(table_manager_create_table(table_manager, self->schema));
  CATCH(error, THROW(error))

  statement_result->type = STATEMENT_RESULT_NONE;
  OK;
}

static void
create_table_statement_destroy(struct i_statement *create_table_statement) {
  struct create_table_statement *self =
      (struct create_table_statement *)create_table_statement;
  //  table_schema_destroy(self->schema);
  free(self);
}

struct create_table_statement *create_table_statement_new() {
  return malloc(sizeof(struct create_table_statement));
}

struct i_statement *
create_table_statement_ctor(struct create_table_statement *self,
                            struct table_schema *schema) {
  self->schema = schema;

  self->parent.execute_impl = create_table_statement_execute;
  self->parent.destroy_impl = create_table_statement_destroy;
  return (struct i_statement *)self;
}

static result_t
drop_table_statement_execute(struct i_statement *drop_table_statement,
                             struct table_manager *table_manager,
                             struct statement_result *statement_result) {
  struct drop_table_statement *self =
      (struct drop_table_statement *)drop_table_statement;

  TRY(table_manager_drop_table(table_manager, self->table_name));
  CATCH(error, THROW(error))

  statement_result->type = STATEMENT_RESULT_NONE;
  OK;
}

static void
drop_table_statement_destroy(struct i_statement *drop_table_statement) {
  struct drop_table_statement *self =
      (struct drop_table_statement *)drop_table_statement;
  free(self);
}

struct drop_table_statement *drop_table_statement_new() {
  return malloc(sizeof(struct drop_table_statement));
}

struct i_statement *drop_table_statement_ctor(struct drop_table_statement *self,
                                              str_t table_name) {
  self->table_name = table_name;
  self->parent.execute_impl = drop_table_statement_execute;
  self->parent.destroy_impl = drop_table_statement_destroy;
  return (struct i_statement *)self;
}

static result_t
query_statement_execute(struct i_statement *query_statement,
                        struct table_manager *table_manager,
                        struct statement_result *statement_result) {
  struct query_statement *self = (struct query_statement *)query_statement;

  struct table *table = NULL;
  TRY(table_manager_get_table(table_manager, self->from, &table));
  CATCH(error, THROW(error))

  struct record_view *view = NULL;
  TRY(table_manager_find(table_manager, table, predicate_clone(self->where),
                         &view));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  statement_result->table = table;
  statement_result->records = view;
  statement_result->type = STATEMENT_RESULT_RECORDS;
  OK;
}

void query_statement_destroy(struct i_statement *query_statement) {
  struct query_statement *self = (struct query_statement *)query_statement;
  predicate_destroy(self->where);
  free(self);
}

struct query_statement *query_statement_new() {
  return malloc(sizeof(struct query_statement));
}

struct i_statement *query_statement_ctor(struct query_statement *self,
                                         str_t from, struct predicate *where) {
  self->from = from;
  self->where = where;

  self->parent.execute_impl = query_statement_execute;
  self->parent.destroy_impl = query_statement_destroy;

  return (struct i_statement *)self;
}

static result_t
insert_statement_execute(struct i_statement *insert_statement,
                         struct table_manager *table_manager,
                         struct statement_result *statement_result) {
  struct insert_statement *self = (struct insert_statement *)insert_statement;

  struct table *table = NULL;
  TRY(table_manager_get_table(table_manager, self->into, &table));
  CATCH(error, THROW(error))

  TRY(table_manager_insert(table_manager, table, self->values));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  table_destroy(table);
  statement_result->type = STATEMENT_RESULT_NONE;
  OK;
}

static void insert_statement_destroy(struct i_statement *insert_statement) {
  struct insert_statement *self = (struct insert_statement *)insert_statement;
  record_group_destroy(self->values);
  free(self);
}

struct insert_statement *insert_statement_new() {
  return malloc(sizeof(struct insert_statement));
}

struct i_statement *insert_statement_ctor(struct insert_statement *self,
                                          str_t into,
                                          struct record_group *values) {
  self->into = into;
  self->values = values;

  self->parent.execute_impl = insert_statement_execute;
  self->parent.destroy_impl = insert_statement_destroy;
  return (struct i_statement *)self;
}
