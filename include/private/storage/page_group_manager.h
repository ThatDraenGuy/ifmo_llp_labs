//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H

#include "public/file/page.h"
#include "public/file/page_manager.h"
#include "public/storage/page_group_manager.h"
#include <stdint.h>

struct application_header {
  uint64_t meta_page;
  uint64_t first_free_page;
  uint64_t last_free_page;
};

struct page_header {
  bool is_free;
  page_id_t next;
  uint8_t contents[];
};

struct page_iterator {
  struct i_page_manager *page_manager;
  page_t current_page;
};

struct page_group_manager {
  struct i_page_manager *page_manager;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H
