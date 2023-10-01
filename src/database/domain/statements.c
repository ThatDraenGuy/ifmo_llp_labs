//
// Created by draen on 25.09.23.
//

#include "private/database/domain/statements.h"
#include <malloc.h>
#include <stdarg.h>

static result_t
create_table_statement_execute(struct i_statement *create_table_statement,
                               struct table_manager *table_manager,
                               struct statement_result *statement_result) {
  struct create_table_statement *self =
      (struct create_table_statement *)create_table_statement;
  statement_result->type = STATEMENT_RESULT_NONE;

  TRY(table_manager_create_table(table_manager, self->schema));
  CATCH(error, THROW(error))

  OK;
}

static void
create_table_statement_destroy(struct i_statement *create_table_statement) {
  struct create_table_statement *self =
      (struct create_table_statement *)create_table_statement;
  free(self);
}

struct i_statement *create_table_statement_of(struct table_schema *schema) {
  struct create_table_statement *self =
      malloc(sizeof(struct create_table_statement));
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
  statement_result->type = STATEMENT_RESULT_NONE;

  TRY(table_manager_drop_table(table_manager, self->table_name));
  CATCH(error, THROW(error))

  OK;
}

static void
drop_table_statement_destroy(struct i_statement *drop_table_statement) {
  struct drop_table_statement *self =
      (struct drop_table_statement *)drop_table_statement;
  free(self);
}

struct i_statement *drop_table_statement_of(str_t table_name) {
  struct drop_table_statement *self =
      malloc(sizeof(struct drop_table_statement));
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
  statement_result->type = STATEMENT_RESULT_RECORDS;

  struct table *main_table = NULL;
  TRY(table_manager_get_table(table_manager, self->from, &main_table));
  CATCH(error, THROW(error))

  struct table **join_tables = malloc(sizeof(void *) * self->joins_num);
  struct predicate **join_predicates = malloc(sizeof(void *) * self->joins_num);

  for (size_t join_index = 0; join_index < self->joins_num; join_index++) {
    struct table *join_table = NULL;
    TRY(table_manager_get_table(
        table_manager, join_get_what(self->joins[join_index]), &join_table));
    CATCH(error, {
      for (size_t index = 0; index < self->joins_num; index++) {
        table_destroy(join_tables[index]);
      }
      free(join_tables);
      free(join_predicates);
      THROW(error);
    })

    join_tables[join_index] = join_table;

    join_predicates[join_index] = join_get_on(self->joins[join_index]);
  }

  struct record_view *view = NULL;
  TRY(table_manager_find_with_joins(
      table_manager, main_table, predicate_clone(self->where), self->joins_num,
      join_tables, join_predicates, &view));
  CATCH(error, {
    table_destroy(main_table);
    for (size_t index = 0; index < self->joins_num; index++) {
      table_destroy(join_tables[index]);
    }
    free(join_tables);
    free(join_predicates);
    THROW(error);
  })

  statement_result->table = main_table;
  statement_result->join_tables = join_tables;
  statement_result->join_predicates = join_predicates;
  statement_result->joins_num = self->joins_num;
  statement_result->records = view;
  OK;
}

void query_statement_destroy(struct i_statement *query_statement) {
  struct query_statement *self = (struct query_statement *)query_statement;
  predicate_destroy(self->where);
  for (size_t index = 0; index < self->joins_num; index++) {
    join_destroy(self->joins[index]);
  }
  free(self->joins);
  free(self);
}

struct i_statement *query_statement_of(str_t from, struct predicate *where,
                                       size_t joins_num, ...) {
  struct query_statement *self = malloc(sizeof(struct query_statement));
  self->from = from;
  self->where = where;
  self->joins_num = joins_num;
  self->joins = malloc(sizeof(void *) * joins_num);

  va_list joins;
  va_start(joins, joins_num);
  for (size_t index = 0; index < joins_num; index++) {
    self->joins[index] = va_arg(joins, struct join *);
  }
  va_end(joins);

  self->parent.execute_impl = query_statement_execute;
  self->parent.destroy_impl = query_statement_destroy;

  return (struct i_statement *)self;
}

static result_t
insert_statement_execute(struct i_statement *insert_statement,
                         struct table_manager *table_manager,
                         struct statement_result *statement_result) {
  struct insert_statement *self = (struct insert_statement *)insert_statement;
  statement_result->type = STATEMENT_RESULT_NONE;

  struct table *table = NULL;
  TRY(table_manager_get_table(table_manager, self->into, &table));
  CATCH(error, THROW(error))

  TRY(table_manager_insert(table_manager, table, self->values));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  table_destroy(table);
  OK;
}

static void insert_statement_destroy(struct i_statement *insert_statement) {
  struct insert_statement *self = (struct insert_statement *)insert_statement;
  record_group_destroy(self->values);
  free(self);
}

struct i_statement *insert_statement_of(str_t into,
                                        struct record_group *values) {
  struct insert_statement *self = malloc(sizeof(struct insert_statement));
  self->into = into;
  self->values = values;

  self->parent.execute_impl = insert_statement_execute;
  self->parent.destroy_impl = insert_statement_destroy;
  return (struct i_statement *)self;
}

static result_t
update_statement_execute(struct i_statement *update_statement,
                         struct table_manager *table_manager,
                         struct statement_result *statement_result) {
  struct update_statement *self = (struct update_statement *)update_statement;
  statement_result->type = STATEMENT_RESULT_NONE;

  struct table *table = NULL;
  TRY(table_manager_get_table(table_manager, self->what, &table));
  CATCH(error, THROW(error))

  TRY(table_manager_update(table_manager, table, self->where, self->set));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  table_destroy(table);
  OK;
}

static void update_statement_destroy(struct i_statement *update_statement) {
  struct update_statement *self = (struct update_statement *)update_statement;
  predicate_destroy(self->where);
  record_update_destroy(self->set);
  free(self);
}

struct i_statement *update_statement_of(str_t what, struct record_update *set,
                                        struct predicate *where) {
  struct update_statement *self = malloc(sizeof(struct update_statement));
  self->what = what;
  self->set = set;
  self->where = where;

  self->parent.execute_impl = update_statement_execute;
  self->parent.destroy_impl = update_statement_destroy;
  return (struct i_statement *)self;
}

static result_t
delete_statement_execute(struct i_statement *delete_statement,
                         struct table_manager *table_manager,
                         struct statement_result *statement_result) {
  struct delete_statement *self = (struct delete_statement *)delete_statement;
  statement_result->type = STATEMENT_RESULT_NONE;

  struct table *table = NULL;
  TRY(table_manager_get_table(table_manager, self->from, &table));
  CATCH(error, THROW(error))

  TRY(table_manager_delete(table_manager, table, self->where));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  TRY(table_manager_flush(table_manager, table));
  CATCH(error, {
    table_destroy(table);
    THROW(error);
  })

  table_destroy(table);
  OK;
}

static void delete_statement_destroy(struct i_statement *delete_statement) {
  struct delete_statement *self = (struct delete_statement *)delete_statement;
  predicate_destroy(self->where);
  free(self);
}

struct i_statement *delete_statement_of(str_t from, struct predicate *where) {
  struct delete_statement *self = malloc(sizeof(struct delete_statement));
  self->from = from;
  self->where = where;

  self->parent.execute_impl = delete_statement_execute;
  self->parent.destroy_impl = delete_statement_destroy;
  return (struct i_statement *)self;
}