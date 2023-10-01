//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H

#include "domain/schema.h"
#include "domain/statement_interface.h"
#include "public/util/result.h"

struct database_manager;

struct database_manager *database_manager_new();

result_t database_manager_ctor(struct database_manager *self, char *file_name);

result_t
database_manager_execute_statement(struct database_manager *self,
                                   struct i_statement *statement,
                                   struct statement_result **statement_result);
void database_manager_finish_statement(struct database_manager *self);

void database_manager_destroy(struct database_manager *self);
#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
