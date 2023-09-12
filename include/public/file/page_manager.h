//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H

#include "public/file/page.h"
#include "public/util/result.h"
#include <stddef.h>

/**
 * Page manager Interface.
 * Under the hood implementations will handle caching & file interaction.
 * Handles memory management of the pages.
 * Implementations must guarantee that contents of a single page are stored in
 * memory in one piece.
 */
struct i_page_manager;

void *page_manager_get_application_header(struct i_page_manager *self);

/**
 * Creates new page and returns its id and data pointer
 * @param self this
 * @param page_data data from the newly created page
 * @return page id
 */
result_t page_manager_create_page(struct i_page_manager *self, page_t *result,
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
result_t page_manager_get_page(struct i_page_manager *self, page_id_t page_id,
                               page_t *result);

/**
 * returns size of a page
 * @param self this
 * @return size of one page
 */
size_t page_manager_get_page_size(struct i_page_manager *self);

/**
 * Saves changes made to the pages content
 * @param self this
 * @return result
 */
result_t page_manager_flush(struct i_page_manager *self);

/**
 * destroys page manager
 * @param self this
 */
void page_manager_destroy(struct i_page_manager *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
