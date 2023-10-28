//
// Created by draen on 12.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERRORS_COMMON_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERRORS_COMMON_H

#define ASSERT_NOT_NULL(Object, ErrorSource)                                   \
  if (Object == NULL)                                                          \
  THROW(error_common(ErrorSource, ERR_COMMON_NULL_POINTER))

#include "common/public/error/error.h"
#include "common/public/util/string.h"

typedef enum {
  ERR_COMMON_NULL_POINTER = 0,
  ERR_COMMON_ITER_OUT_OF_RANGE,
  ERR_INDEX_OUT_OF_BOUNDS
} common_error_code_t;

extern const str_t common_error_type;

extern const str_t common_error_messages[];

static inline struct error *
error_common(str_t error_source, common_error_code_t common_error_code) {
  return error_new(error_source, common_error_type,
                   (error_code_t){common_error_code},
                   common_error_messages[common_error_code]);
}

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERRORS_COMMON_H
