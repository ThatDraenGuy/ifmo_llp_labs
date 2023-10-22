//
// Created by draen on 25.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENTS_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENTS_H

#include "database/public/database/domain/statement/statements.h"
#include "statement_interface.h"

struct create_table_statement {
  struct i_statement parent;
  struct table_schema *schema;
};

struct drop_table_statement {
  struct i_statement parent;
  str_t table_name;
};

struct query_statement {
  struct i_statement parent;
  str_t from;
  struct predicate *where;
  size_t joins_num;
  struct join **joins;
};

struct insert_statement {
  struct i_statement parent;
  str_t into;
  struct record_group *values;
};

struct update_statement {
  struct i_statement parent;
  str_t what;
  struct record_update *set;
  struct predicate *where;
};

struct delete_statement {
  struct i_statement parent;
  str_t from;
  struct predicate *where;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENTS_H
