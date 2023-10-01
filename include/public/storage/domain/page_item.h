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
  uint16_t flags;
};

typedef struct page_item_id_data *page_item_id_t;

static inline size_t page_item_entry_size(size_t item_size) {
  return item_size + sizeof(struct page_item_id_data);
}

#define PAGE_ITEM_DELETED_MASK 0b0000000000000001
#define PAGE_ITEM_NEW_MASK 0b0000000000000010

static inline bool page_item_is_deleted(page_item_id_t page_item_id) {
  return (page_item_id->flags & PAGE_ITEM_DELETED_MASK) != 0;
}
static inline void page_item_set_deleted(page_item_id_t page_item_id) {
  page_item_id->flags |= PAGE_ITEM_DELETED_MASK;
}
static inline void page_item_unset_deleted(page_item_id_t page_item_id) {
  page_item_id->flags &= !PAGE_ITEM_DELETED_MASK;
}

static inline bool page_item_is_new(page_item_id_t page_item_id) {
  return (page_item_id->flags & PAGE_ITEM_NEW_MASK) != 0;
}
static inline void page_item_set_new(page_item_id_t page_item_id) {
  page_item_id->flags |= PAGE_ITEM_NEW_MASK;
}

static inline void page_item_unset_new(page_item_id_t page_item_id) {
  page_item_id->flags &= !PAGE_ITEM_NEW_MASK;
}

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_ITEM_H
