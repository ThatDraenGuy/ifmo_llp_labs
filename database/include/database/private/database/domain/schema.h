//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H

#include "common/public/util/string.h"
#include "database/public/database/domain/schema.h"

struct column_schema {
  string_t name;
  str_t table_name;
  column_type_t type;
};

struct column_schema_iterator {
  size_t current_index;
  struct column_schema_group *column_schema_group;
};

struct table_schema {
  string_t table_name;
  size_t current_column_index;
  struct column_schema_group column_schema_group;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SCHEMA_H
