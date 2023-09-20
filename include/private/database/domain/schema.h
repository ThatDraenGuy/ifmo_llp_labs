//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H

#include "public/database/domain/schema.h"

struct column_schema {
  char *name;
  column_type_t type;
};

struct column_schema_iterator {
  size_t current_index;
  size_t column_amount;
  struct column_schema *columns;
};

struct table_schema {
  char *table_name;
  size_t column_amount;
  size_t current_column_index;
  struct column_schema *columns;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H
