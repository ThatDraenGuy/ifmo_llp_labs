//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H

#include "public/database/domain/predicate.h"
#include "public/database/domain/record/record_group.h"
#include "public/database/domain/record/record_view.h"
#include "public/database/table_manager.h"
#include "public/storage/page_data_manager.h"

struct record_view {
  struct table_manager *table_manager;
  struct table *main_table;
  struct item_iterator *main_item_it;
  struct predicate *where;
  size_t joins_num;
  struct table **join_tables;
  struct predicate **join_predicates;
  bool is_empty;
  struct single_record_holder *current_record_holder;
  struct single_record_holder *next_record_holder;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_RECORD_VIEW_H
