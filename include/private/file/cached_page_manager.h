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

struct cache_entry {
  uint8_t relevancy_value;
  bool is_altered;
  page_id_t page_id;
  uint8_t contents[];
};

struct cached_page_manager {
  struct i_page_manager parent;
  struct i_page_resolver *page_resolver;
  cache_entry_index_t cache_size;
  struct cache_entry *cache;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_CACHED_PAGE_MANAGER_H
