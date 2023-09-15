//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_DATA_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_DATA_MANAGER_H

#include "domain/item.h"
#include "domain/page_group.h"
#include "public/file/domain/page.h"
#include "public/util/result.h"
#include <stdbool.h>
#include <stddef.h>

struct page_data_manager;
struct item_iterator;

struct page_data_manager *page_data_manager_new();

result_t page_data_manager_ctor(struct page_data_manager *self,
                                char *file_name);

result_t page_data_manager_create_group(struct page_data_manager *self,
                                        page_group_id_t *result);

result_t page_data_manager_insert(struct page_data_manager *self,
                                  page_group_id_t page_group_id, item_t item);

struct item_iterator *
page_data_manager_get_items(struct page_data_manager *self,
                            page_group_id_t page_group_id);

result_t page_data_manager_flush(struct page_data_manager *self,
                                 page_group_id_t page_group_id);
void page_data_manager_destroy(struct page_data_manager *self);

bool item_iterator_has_next(struct item_iterator *self);
result_t item_iterator_next(struct item_iterator *self, item_t *result);
void item_iterator_delete_item(struct item_iterator *self);
void item_iterator_destroy(struct item_iterator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_STORAGE_PAGE_DATA_MANAGER_H
