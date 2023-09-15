//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H

#include "public/file/domain/page.h"
#include "public/file/page_manager.h"
#include "public/storage/page_group_manager.h"
#include <stdint.h>

struct __attribute__((packed)) application_header {
  page_id_t meta_page;
  page_id_t first_free_page;
  page_id_t last_free_page;
};

struct __attribute__((packed)) page_header {
  page_id_t next;
  uint8_t contents[];
};

struct page_iterator {
  struct page_manager *page_manager;
  page_id_t next_page_id;
  page_t current_page;
};

struct page_group_manager {
  struct page_manager *page_manager;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_STORAGE_PAGE_GROUP_MANAGER_H
