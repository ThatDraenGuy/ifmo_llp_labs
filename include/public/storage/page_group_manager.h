//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H

#include "public/storage/page_group.h"
#include "public/util/result.h"

struct page_group_manager;

struct page_iterator;

struct page_group_manager *page_group_manager_new();

result_t page_group_manager_ctor(struct page_group_manager *self,
                                 char *file_name);

size_t page_group_manager_get_page_size(struct page_group_manager *self);

struct page_iterator *
page_group_manager_get_group(struct page_group_manager *self,
                             page_group_id_t page_group_id);

result_t page_group_manager_add_page(struct page_group_manager *self,
                                     struct page_iterator *it, page_t *result);

struct page_iterator *
page_group_manager_get_meta_group(struct page_group_manager *self);

bool page_iterator_has_next(struct page_iterator *self);
result_t page_iterator_next(struct page_iterator *self, page_t *result);
void page_iterator_destroy(struct page_iterator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H
