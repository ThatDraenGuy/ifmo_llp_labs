//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H

#include "private/file/page_manager.h"
#include "public/file/page_manager.h"
#include "public/file/page_resolver.h"
#include <stdint.h>

typedef struct {
  size_t bytes;
} cache_entry_index_t;

typedef struct {
  size_t bytes;
} relevancy_value_t;

#define RELEVANCY_VALUE_MAX                                                    \
  (relevancy_value_t) { .bytes = SIZE_MAX }

#define LEAST_RELEVANT_VALUE                                                   \
  (relevancy_value_t) { .bytes = 0 }

struct cache_entry {
  relevancy_value_t relevancy_value;
  bool is_altered;
  page_id_t page_id;
  uint8_t contents[];
};

struct page_manager {
  struct page_resolver *page_resolver;
  relevancy_value_t current_most_relevant_value;
  page_id_t last_accessed_page;
  cache_entry_index_t cache_size;
  uint8_t *cache;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
