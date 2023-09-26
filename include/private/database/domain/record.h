//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H

#include "private/database/domain/schema.h"
#include "public/database/domain/record.h"
#include "public/storage/domain/page_group.h"
#include "public/util/queue.h"
#include "schema.h"

struct record_entry {
  struct column_schema schema;
  column_value_t value;
};

struct record {
  struct queue *entries;
};

result_t record_copy_into(struct record *self, struct record *target);

result_t record_insert_value(struct record *self, char *column_name,
                             column_value_t value, column_type_t type);

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_H
