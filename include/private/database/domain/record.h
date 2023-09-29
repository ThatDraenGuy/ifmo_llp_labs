//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H

#include "public/database/domain/record.h"
#include "public/database/domain/types.h"

struct record {
  struct column_schema_group *column_schema_group;
  column_value_t values[];
};

struct record *record_new(struct column_schema_group *column_schema_group);
void record_destroy(struct record *self);

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
