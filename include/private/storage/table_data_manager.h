//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_STORAGE_TABLE_DATA_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_STORAGE_TABLE_DATA_MANAGER_H

#include "public/file/page_manager.h"
#include "public/storage/table_data_manager.h"
#include <stdint.h>

typedef size_t page_index;

struct table_page_header {
  page_index free_space_start;
  page_index free_space_end;
  uint8_t contents[];
};

struct table_data_manager {
  struct i_page_manager *page_manager;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_STORAGE_TABLE_DATA_MANAGER_H
