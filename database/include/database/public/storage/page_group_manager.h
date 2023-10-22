//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H

#include "common/public/util/result.h"
#include "domain/page_group.h"

struct page_group_manager;

struct page_iterator;

struct page_group_manager *page_group_manager_new();

result_t page_group_manager_ctor(struct page_group_manager *self,
                                 char *file_name);

size_t page_group_manager_get_page_capacity(struct page_group_manager *self);

struct page_iterator *
page_group_manager_get_group(struct page_group_manager *self,
                             page_group_id_t page_group_id, bool inverse);
result_t page_group_manager_get_page(struct page_group_manager *self,
                                     page_id_t page_id, page_t *result);

result_t page_group_manager_add_page(struct page_group_manager *self,
                                     page_group_id_t page_group_id,
                                     page_t *result);

result_t page_group_manager_create_group(struct page_group_manager *self,
                                         page_group_id_t *result);
result_t page_group_manager_free_page_current(struct page_group_manager *self,
                                              struct page_iterator *it);
result_t page_group_manager_delete_group(struct page_group_manager *self,
                                         page_group_id_t page_group_id);

page_group_id_t
page_group_manager_get_meta_group_id(struct page_group_manager *self);

result_t page_group_manager_set_meta_group_id(struct page_group_manager *self,
                                              page_group_id_t page_group_id);

result_t page_group_manager_flush(struct page_group_manager *self);
void page_group_manager_destroy(struct page_group_manager *self);

bool page_iterator_has_next(struct page_iterator *self);
result_t page_iterator_current(struct page_iterator *self, page_t *result);
page_id_t page_iterator_current_id(struct page_iterator *self);
result_t page_iterator_next(struct page_iterator *self, page_t *result);
void page_iterator_destroy(struct page_iterator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_GROUP_MANAGER_H
