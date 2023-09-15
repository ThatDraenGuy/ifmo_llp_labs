//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H

#include "page_item.h"
#include "public/file/domain/page.h"

typedef struct {
  size_t size;
  void *data;
} item_t;

#define ITEM_NULL                                                              \
  (item_t) { .size = 0, .data = NULL }

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H
