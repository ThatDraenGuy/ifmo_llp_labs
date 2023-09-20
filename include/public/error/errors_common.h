//
// Created by draen on 12.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_ERROR_ERRORS_COMMON_H
#define LLP_LAB_INCLUDE_PUBLIC_ERROR_ERRORS_COMMON_H

#define ASSERT_NOT_NULL(Object, ErrorSource)                                   \
  if (Object == NULL)                                                          \
  THROW(error_common(ErrorSource, ERR_COMMON_NULL_POINTER))

#include "public/error/error.h"

typedef enum {
  ERR_COMMON_NULL_POINTER = 0,
  ERR_COMMON_ITER_OUT_OF_RANGE
} common_error_code_t;

extern const char *const common_error_type;

extern const char *const common_error_messages[];

static inline struct error *
error_common(const char *const error_source,
             common_error_code_t common_error_code) {
  return error_new(error_source, common_error_type,
                   (error_code_t){common_error_code},
                   common_error_messages[common_error_code]);
}

#endif // LLP_LAB_INCLUDE_PUBLIC_ERROR_ERRORS_COMMON_H
