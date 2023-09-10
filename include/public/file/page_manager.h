//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H

#include "public/file/page.h"
#include "public/util/iterator.h"
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

/**
 * Creates new page and returns its id and data pointer
 * @param page_manager this
 * @param page_data data from the newly created page
 * @return page id
 */
page_id_t page_manager_create_page(struct i_page_manager *page_manager,
                                   page_t *result);

/**
 * Returns data from specified page.
 * Data is safe to read and modify.
 * Any changes made to page content are only guaranteed to be saved after
 * calling page_manager_flush(...)
 * @param page_manager this
 * @param page_id page id
 * @param result data from the page
 * @return result
 */
result_t page_manager_get_page(struct i_page_manager *page_manager,
                               page_id_t page_id, page_t *result);

/**
 * Returns iterator of data from all the pages.
 * @param page_manager this
 * @param iterator page data iterator (result)
 * @return result
 */
result_t page_manager_iterator(struct i_page_manager *page_manager,
                               struct iterator *iterator);

/**
 * returns size of a page
 * @param page_manager this
 * @return size of one page
 */
size_t page_manager_get_page_size(struct i_page_manager *page_manager);

/**
 * Saves changes made to the pages content
 * @param page_manager this
 * @return result
 */
result_t page_manager_flush(struct i_page_manager *page_manager);

/**
 * destroys page manager
 * @param page_manager this
 */
void page_manager_destroy(struct i_page_manager *page_manager);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_MANAGER_H
