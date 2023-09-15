//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_ITEM_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_ITEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint16_t bytes;
} page_index_t;

struct page_item_id_data {
  page_index_t item_offset;
  page_index_t item_size;
  uint16_t is_deleted;
};

typedef struct page_item_id_data *page_item_id_t;

static inline size_t page_item_entry_size(size_t item_size) {
  return item_size + sizeof(struct page_item_id_data);
}

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_ITEM_H
