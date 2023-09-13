//
// Created by draen on 09.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H

#include "public/file/page.h"
#include "public/util/result.h"

/**
 * Page resolver interface.
 * Handles reading and writing pages of uniform size.
 * Implementations do NOT handle memory management of loaded pages
 */
struct i_page_resolver;

size_t page_resolver_get_page_size(struct i_page_resolver *self);
void *page_resolver_get_application_header(struct i_page_resolver *self);
result_t page_resolver_get_new_page_id(struct i_page_resolver *self,
                                       page_id_t *result_id);
result_t page_resolver_read_page(struct i_page_resolver *self,
                                 page_id_t page_id, page_t destination);
result_t page_resolver_write_page(struct i_page_resolver *self,
                                  page_id_t page_id, page_t data);
void page_resolver_destroy(struct i_page_resolver *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H
