//
// Created by draen on 23.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENT_INTERFACE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENT_INTERFACE_H

#include "public/database/database_manager.h"
#include "public/database/domain/statement/statement_interface.h"
#include "public/database/table_manager.h"

typedef enum {
  STATEMENT_RESULT_NONE,
  STATEMENT_RESULT_RECORDS,
} statement_result_type_t;

struct statement_result {
  statement_result_type_t type;
  struct table *table;
  size_t joins_num;
  struct table **join_tables;
  struct predicate **join_predicates;
  union {
    struct record_view *records;
  };
};

struct i_statement {
  result_t (*execute_impl)(struct i_statement *self,
                           struct table_manager *table_manager,
                           struct statement_result *statement_result);
  void (*destroy_impl)(struct i_statement *self);
};

result_t statement_execute(struct i_statement *self,
                           struct table_manager *table_manager,
                           struct statement_result *statement_result);

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_STATEMENT_INTERFACE_H
