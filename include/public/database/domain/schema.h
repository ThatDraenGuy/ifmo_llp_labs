//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H

#include "public/util/result.h"
#include "public/util/string.h"
#include "types.h"
#include <stdbool.h>

struct column_schema;
column_type_t column_schema_get_type(struct column_schema *self);
str_t column_schema_get_name(struct column_schema *self);

struct column_schema_group {
  size_t columns_amount;
  struct column_schema **schemas;
};

struct table_schema;
struct column_schema_iterator;

struct table_schema *table_schema_new();

void table_schema_ctor(struct table_schema *self, str_t table_name,
                       size_t columns_amount);

str_t table_schema_get_name(struct table_schema *self);

size_t table_schema_get_column_amount(struct table_schema *self);

result_t table_schema_add_column(struct table_schema *self, str_t column_name,
                                 column_type_t column_type);

struct column_schema_iterator *
table_schema_get_columns(struct table_schema *self);

void table_schema_destroy(struct table_schema *self);

bool column_schema_iterator_has_next(struct column_schema_iterator *self);
result_t column_schema_iterator_next(struct column_schema_iterator *self,
                                     struct column_schema **result);
void column_schema_iterator_destroy(struct column_schema_iterator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H
