//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H

#include "page.h"
#include "public/util/result.h"

struct page_resolver;

struct page_resolver *page_resolver_new();

result_t page_resolver_ctor(struct page_resolver *self, char *file_name,
                            size_t application_header_size,
                            void *default_header);

size_t page_resolver_get_page_size(struct page_resolver *self);
void *page_resolver_get_application_header(struct page_resolver *self);
result_t page_resolver_get_new_page_id(struct page_resolver *self,
                                       page_id_t *result_id);
result_t page_resolver_read_page(struct page_resolver *self, page_id_t page_id, page_t destination);
result_t page_resolver_write_page(struct page_resolver *self, page_id_t page_id, page_t data);
void page_resolver_destroy(struct page_resolver *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_PAGE_RESOLVER_H
