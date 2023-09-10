//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_PAGE_RESOLVER_H

#include "private/file/page_resolver.h"
#include "public/file/file_manager.h"
#include "public/file/file_page_resolver.h"
#include <stdint.h>

#define FORMAT_TYPE 0xB0BA

struct __attribute__((packed)) file_header {
  uint16_t format_type;
  uint32_t offset_to_data;
  uint32_t page_size;
  uint32_t page_amount;
};

struct file_page_resolver {
  struct i_page_resolver parent;
  struct file_manager *file_manager;
  struct file_header file_header;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_PAGE_RESOLVER_H
