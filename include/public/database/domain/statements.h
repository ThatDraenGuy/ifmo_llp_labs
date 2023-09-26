//
// Created by draen on 25.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H

#include "statement_interface.h"

struct create_table_statement;
struct create_table_statement *create_table_statement_new();
struct i_statement *
create_table_statement_ctor(struct create_table_statement *self,
                            struct table_schema *schema);

struct drop_table_statement;
struct drop_table_statement *drop_table_statement_new();
struct i_statement *drop_table_statement_ctor(struct drop_table_statement *self,
                                              char *table_name);

struct query_statement;
struct query_statement *query_statement_new();
struct i_statement *query_statement_ctor(struct query_statement *self,
                                         char *from, struct predicate *where);

struct insert_statement;
struct insert_statement *insert_statement_new();
struct i_statement *insert_statement_ctor(struct insert_statement *self,
                                          char *into, struct record *values);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENTS_H
