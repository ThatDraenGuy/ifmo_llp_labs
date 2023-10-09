//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_GROUP_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_GROUP_H

#include "public/database/domain/record/record_group.h"
#include "public/util/queue.h"

struct record_iterator {
  struct queue_iterator *it;
};

struct record_group {
  struct column_schema_group column_schema_group;
  struct queue *records;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_GROUP_H
