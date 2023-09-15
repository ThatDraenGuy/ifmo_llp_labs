//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H

#include "public/database/table_manager.h"
#include "public/storage/page_data_manager.h"

struct table_manager {
  struct page_data_manager *page_data_manager;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
