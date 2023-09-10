//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H

#include "public/file/page_manager.h"

struct i_page_manager {
  size_t (*get_page_size_impl)(void *this);
  result_t (*get_page_impl)(void *this, page_id_t page_id, page_t *result);
  void (*destroy_impl)(void *this);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_MANAGER_H
