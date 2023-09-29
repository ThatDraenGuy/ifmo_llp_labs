//
// Created by draen on 28.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H

#include "public/database/domain/single_record_holder.h"

struct single_record_holder {
  struct column_schema_group column_schema_group;
  struct record *record;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
