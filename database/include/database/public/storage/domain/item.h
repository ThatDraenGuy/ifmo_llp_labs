//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H

#include "database/public/file/domain/page.h"
#include "page_item.h"

typedef struct {
  size_t size;
  void *data;
} item_t;

#define ITEM_NULL                                                              \
  (item_t) { .size = 0, .data = NULL }

void item_destroy(item_t item);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_ITEM_H
