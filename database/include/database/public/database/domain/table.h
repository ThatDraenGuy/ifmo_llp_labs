//
// Created by draen on 18.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TABLE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TABLE_H

#include "database/public/database/domain/schema.h"
#include "database/public/storage/domain/page_group.h"

struct table;

struct table *table_new();
void table_ctor(struct table *self, struct table_schema *schema,
                page_group_id_t page_group_id);
struct table_schema *table_get_schema(struct table *self);
void table_destroy(struct table *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_TABLE_H
