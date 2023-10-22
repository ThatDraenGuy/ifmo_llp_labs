//
// Created by draen on 25.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H

#include "database/public/database/domain/join.h"
#include "database/public/database/domain/record/record_group.h"
#include "database/public/database/domain/record/record_update.h"
#include "statement_interface.h"

struct i_statement *create_table_statement_of(struct table_schema *schema);

struct i_statement *drop_table_statement_of(str_t table_name);

struct i_statement *query_statement_of(str_t from, struct predicate *where,
                                       size_t joins_num, ...);

struct i_statement *insert_statement_of(str_t into,
                                        struct record_group *values);

struct i_statement *update_statement_of(str_t what, struct record_update *set,
                                        struct predicate *where);

struct i_statement *delete_statement_of(str_t from, struct predicate *where);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H
