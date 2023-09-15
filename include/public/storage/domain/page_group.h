//
// Created by draen on 11.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H

#include <stddef.h>

typedef struct {
  size_t bytes;
} page_group_id_t;

#define PAGE_GROUP_ID_NULL                                                     \
  (page_group_id_t) { .bytes = 0 }

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_DOMAIN_PAGE_GROUP_H
