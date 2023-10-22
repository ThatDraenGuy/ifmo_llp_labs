//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_STRING_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_STRING_H

#include "common/public/util/string.h"

struct str {
  string_size_t size;
  char data[];
};

struct string {
  string_size_t capacity;
  string_size_t size;
  char data[];
};

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_STRING_H
