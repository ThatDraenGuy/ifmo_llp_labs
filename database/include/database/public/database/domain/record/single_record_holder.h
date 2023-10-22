//
// Created by draen on 28.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H

#include "database/public/database/domain/record/record.h"
#include "database/public/database/domain/schema.h"
#include "database/public/database/domain/table.h"

struct single_record_holder;
struct single_record_holder *single_record_holder_new();
void single_record_holder_ctor(struct single_record_holder *self,
                               size_t table_schema_num, ...);
void single_record_holder_ctor_with_joins(struct single_record_holder *self,
                                          struct table *main_table,
                                          size_t joins_num,
                                          struct table **join_tables);
void single_record_holder_ctor_from_columns(struct single_record_holder *self,
                                            size_t columns_num,
                                            struct column_schema **schemas);
struct record *single_record_holder_get(struct single_record_holder *self);
void single_record_holder_clear(struct single_record_holder *self,
                                size_t first_column, size_t last_column);
void single_record_holder_clear_all(struct single_record_holder *self);
void single_record_holder_destroy(struct single_record_holder *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
