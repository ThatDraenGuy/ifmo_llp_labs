//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H

struct column_schema;

typedef enum {
  COLUMN_TYPE_INT32,
  COLUMN_TYPE_FLOAT,
  COLUMN_TYPE_STRING,
  COLUMN_TYPE_BOOL,
} column_type_t;

struct table_schema;

struct table_schema *table_schema_new();
void table_schema_add_column(struct table_schema *self, char *column_name,
                             column_type_t column_type);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SCHEMA_H
