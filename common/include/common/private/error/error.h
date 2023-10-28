//
// Created by draen on 12.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_ERROR_ERROR_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_ERROR_ERROR_H

#include "common/public/error/error.h"

struct error {
  string_t error_source;
  string_t error_type;
  error_code_t error_code;
  string_t error_message;
};

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_ERROR_ERROR_H
