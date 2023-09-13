//
// Created by draen on 09.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H

#include "public/file/page_resolver.h"

struct i_page_resolver {
  size_t (*get_page_size_impl)(void *self);
  void *(*get_application_header_impl)(void *self);
  result_t (*get_new_page_id_impl)(void *self, page_id_t *result);
  result_t (*read_page_impl)(void *self, page_id_t page_id, page_t destination);
  result_t (*write_page_impl)(void *self, page_id_t page_id, page_t data);
  void (*destroy_impl)(void *self);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H
