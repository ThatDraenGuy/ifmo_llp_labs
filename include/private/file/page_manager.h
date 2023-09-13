//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H

#include "public/file/page_manager.h"

struct i_page_manager {
  void *(*get_application_header_impl)(void *self);
  result_t (*create_page_impl)(void *self, page_t *result,
                               page_id_t *result_id);
  size_t (*get_page_size_impl)(void *self);
  result_t (*get_page_impl)(void *self, page_id_t page_id, page_t *result);
  result_t (*flush_impl)(void *self);
  void (*destroy_impl)(void *self);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
