//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H

#include "domain/record_iterator.h"
#include "domain/schema.h"
#include "domain/statement.h"
#include "public/util/result.h"

struct database_manager;

struct database_manager *database_manager_new();

result_t database_manager_ctor(struct database_manager *self, char *file_name);

result_t database_manager_create_table(struct database_manager *self,
                                       struct table_schema *schema);
result_t database_manager_drop_table(struct database_manager *self,
                                     char *table_name);

result_t database_manager_execute_query(
    struct database_manager *self, struct query_statement *query,
    struct record_iterator **result); // TODO type shenanigans

result_t database_manager_execute_update(struct database_manager *self,
                                         struct update_statement *update,
                                         affected_records_num_t *result);

void database_manager_destroy(struct database_manager *self);
#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
