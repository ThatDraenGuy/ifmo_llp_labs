//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_CACHED_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_CACHED_PAGE_MANAGER_H

#include "private/file/page_manager.h"
#include "public/file/cached_page_manager.h"
#include "public/file/page_resolver.h"
#include <stdint.h>

typedef struct {
  size_t bytes;
} cache_entry_index_t;

typedef struct {
  uint8_t bytes;
} relevancy_value_t;

#define MOST_RELEVANT_VALUE                                                    \
  (relevancy_value_t) { .bytes = UINT8_MAX }

#define LEAST_RELEVANT_VALUE                                                   \
  (relevancy_value_t) { .bytes = 0 }

struct cache_entry {
  relevancy_value_t relevancy_value;
  bool is_altered;
  page_id_t page_id;
  uint8_t contents[];
};

struct cached_page_manager {
  struct i_page_manager parent;
  struct i_page_resolver *page_resolver;
  cache_entry_index_t cache_size;
  uint8_t *cache;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_CACHED_PAGE_MANAGER_H
