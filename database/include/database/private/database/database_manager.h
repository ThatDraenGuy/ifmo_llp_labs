//
// Created by draen on 23.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DATABASE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DATABASE_MANAGER_H

#include "database/public/database/database_manager.h"
#include "database/public/database/table_manager.h"

struct database_manager {
  struct table_manager *table_manager;
  struct i_statement *current_statement;
  struct statement_result *current_result;
  bool was_statement_successful;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DATABASE_MANAGER_H
