//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_DATA_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_DATA_MANAGER_H

#include "public/storage/page_data_manager.h"
#include "public/storage/page_group_manager.h"
#include <stddef.h>
#include <stdint.h>

struct item_iterator {
  struct page_iterator *page_iterator;
  page_t current_page;
  bool is_empty;
  uint16_t next_item_index;
  item_t current_item;
};

struct __attribute__((packed)) page_data_header {
  uint16_t item_amount;
  page_index_t free_space_start;
  page_index_t free_space_end;
  uint8_t contents[];
};

struct page_data_manager {
  struct page_group_manager *page_group_manager;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_DATA_MANAGER_H
