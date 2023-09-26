//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENT_INTERFACE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENT_INTERFACE_H

#include "predicate.h"
#include "record_iterator.h"
#include <stddef.h>

struct statement_result;
struct statement_result *statement_result_new();
void statement_result_clear(struct statement_result *self);
struct record_iterator *statement_result_records(struct statement_result *self);
void statement_result_destroy(struct statement_result *self);

struct i_statement;
void statement_destroy(struct i_statement *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_STATEMENT_INTERFACE_H
