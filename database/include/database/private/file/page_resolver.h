//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H

#include "database/private/file/page_resolver.h"
#include "database/public/file/file_manager.h"
#include "database/public/file/page_resolver.h"
#include <stdint.h>

#define FORMAT_TYPE 0xB0BA

struct __attribute__((packed)) file_header {
  uint16_t format_type;
  uint32_t offset_to_data;
  uint64_t page_size;
  uint64_t page_amount;
};

struct page_resolver {
  struct file_manager *file_manager;
  struct file_header file_header;
  bool was_file_header_altered;
  size_t application_header_size;
  void *application_header;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_PAGE_RESOLVER_H
