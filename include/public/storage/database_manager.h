//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H

#include "public/domain/schema/table_schema.h"
#include "public/storage/page_data_manager.h"
#include "public/util/result.h"

struct database_manager;

struct database_manager *database_manager_new();

result_t database_manager_ctor(struct database_manager *self, char *file_name);

struct page_data_manager *
database_manager_create_table(struct database_manager *self,
                              struct table_schema *table_schema);

struct page_data_manager *
database_manager_get_table(struct database_manager *self, char *table_name);

void database_manager_delete_table(struct database_manager *self,
                                   char *table_name);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DATABASE_MANAGER_H
