//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H

#include "common/public/domain/column_types.h"
#include "database/public/database/domain/record/record.h"

struct record {
  struct column_schema_group *column_schema_group;
  column_value_t values[];
};

struct record *record_new(struct column_schema_group *column_schema_group);
void record_destroy(struct record *self);

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
