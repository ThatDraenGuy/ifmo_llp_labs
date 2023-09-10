//
// Created by draen on 09.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_H

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_PAGE_SIZE 1024

/// page id type. Uniquely identifies a page
typedef struct {
  size_t bytes;
} page_id_t;

typedef struct {
  void *data;
} page_t;

// static inline page_t page_from(void *data) { return (page_t){data}; }

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_H
