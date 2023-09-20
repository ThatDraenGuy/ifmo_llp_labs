//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H

#include "page_resolver.h"
#include "public/util/result.h"

struct page_manager;

struct page_manager *page_manager_new();

result_t page_manager_ctor(struct page_manager *self,
                           struct page_resolver *page_resolver,
                           size_t cache_size);

void *page_manager_get_application_header(struct page_manager *self);
result_t page_manager_flush_application_header(struct page_manager *self);

/**
 * Creates new page and returns its id and data pointer
 * @param self this
 * @param page_data data from the newly created page
 * @return page id
 */
result_t page_manager_create_page(struct page_manager *self, page_t *result,
                                  page_id_t *result_id);

/**
 * Returns data from specified page.
 * Data is safe to read and modify.
 * Any changes made to page content are only guaranteed to be saved after
 * calling page_manager_flush(...)
 * @param self this
 * @param page_id page id
 * @param result data from the page
 * @return result
 */
result_t page_manager_get_page(struct page_manager *self, page_id_t page_id,
                               page_t *result);

/**
 * returns size of a page
 * @param self this
 * @return size of one page
 */
size_t page_manager_get_page_size(struct page_manager *self);

/**
 * Saves changes made to the pages content
 * @param self this
 * @return result
 */
result_t page_manager_flush(struct page_manager *self);

/**
 * destroys page manager
 * @param self this
 */
void page_manager_destroy(struct page_manager *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
