//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t bytes;
} page_group_id_t;

#define PAGE_GROUP_ID_NULL                                                     \
  (page_group_id_t) { .bytes = 0 }

static inline bool page_group_id_is_null(page_group_id_t page_group_id) {
  return page_group_id.bytes == PAGE_GROUP_ID_NULL.bytes;
}

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H
