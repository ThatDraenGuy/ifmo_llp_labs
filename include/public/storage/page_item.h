//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_ITEM_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_ITEM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t bytes;
} page_index_t;

typedef struct {
  uint16_t item_offset;
  uint16_t item_size;
  bool is_deleted;
} page_item_id_data_t;

typedef page_item_id_data_t *page_item_id_t;

// typedef struct {
//   void *data;
// } page_item_t;

static inline size_t page_item_entry_size(size_t item_size) {
  return item_size + sizeof(page_item_id_data_t);
}

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_ITEM_H
