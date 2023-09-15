//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_TABLE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_TABLE_MANAGER_H

#include "domain/data.h"
#include "domain/predicate.h"
#include "domain/record_iterator.h"
#include "domain/schema.h"
#include "public/util/result.h"

struct table_manager;

struct table_manager *table_manager_new();

result_t table_manager_ctor(struct table_manager *self, char *file_name);

result_t table_manager_create_table(struct table_manager *self,
                                    struct table_schema *schema,
                                    struct table *result);

result_t table_manager_get_table(struct table_manager *self, char *table_name,
                                 struct table *result);

result_t
table_manager_find(struct table_manager *self, struct table *table,
                   struct predicate *predicate,
                   struct record_iterator **result); // TODO type shenanigans

result_t table_manager_insert(struct table_manager *self, struct table *table,
                              struct record *record);

result_t table_manager_update(struct table_manager *self, struct table *table,
                              struct predicate *predicate,
                              struct record_update *record_update);

result_t table_manager_delete(struct table_manager *self, struct table *table,
                              struct predicate *predicate);

void table_manager_destroy(struct table_manager *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_TABLE_MANAGER_H
