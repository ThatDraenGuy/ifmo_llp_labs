//
// Created by draen on 09.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_DOMAIN_PAGE_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_DOMAIN_PAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_PAGE_SIZE 1024

/// page id type. Uniquely identifies a page
typedef struct {
  uint64_t bytes;
} page_id_t;

#define PAGE_ID_NULL                                                           \
  (page_id_t) { .bytes = 0 }

static inline bool page_id_is_null(page_id_t page_id) {
  return page_id.bytes == PAGE_ID_NULL.bytes;
};

typedef struct {
  void *data;
} page_t;

#define PAGE_NULL                                                              \
  (page_t) { .data = NULL }

static inline bool page_is_null(page_t page) {
  return page.data == PAGE_NULL.data;
}

// static inline page_t page_from(void *data) { return (page_t){data}; }

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_DOMAIN_PAGE_H
