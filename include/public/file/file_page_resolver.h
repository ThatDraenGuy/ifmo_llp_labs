//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_PAGE_RESOLVER_H

#include "public/util/result.h"

struct file_page_resolver;

struct file_page_resolver *file_page_resolver_new();

result_t file_page_resolver_ctor(struct file_page_resolver *self,
                                 char *file_name, size_t header_size,
                                 void *default_header);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_PAGE_RESOLVER_H
