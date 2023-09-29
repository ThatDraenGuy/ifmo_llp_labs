//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H

#include "public/database/domain/predicate.h"
#include "public/database/domain/record_group.h"
#include "public/database/domain/record_view.h"
#include "public/storage/page_data_manager.h"

struct record_view {
  struct item_iterator *item_it;
  struct predicate *predicate;
  struct table_schema *schema;
  bool is_empty;
  struct single_record_holder *current_record_holder;
  struct single_record_holder *next_record_holder;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H
