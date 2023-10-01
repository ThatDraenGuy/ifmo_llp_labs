//
// Created by draen on 30.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_UPDATE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_UPDATE_H

#include "public/database/domain/record_update.h"
#include "public/database/domain/single_record_holder.h"

struct record_update {
  bool applicable_in_place;
  string_t table_name;
  size_t columns_num;
  struct column_schema **schemas;
  struct single_record_holder *update_holder;
};

result_t record_update_apply(struct record_update *self, struct record *target);

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_UPDATE_H
