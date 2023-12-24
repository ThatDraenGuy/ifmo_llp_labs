//
// Created by draen on 12.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERROR_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERROR_H

#include "common/public/util/string.h"
#include <stddef.h>
typedef struct {
  size_t bytes;
} error_code_t;

struct error;

struct error *error_new(str_t error_source, str_t error_type,
                        error_code_t error_code, str_t error_message);
str_t error_get_source(struct error *self);
str_t app_error_get_type(struct error *self);
error_code_t error_get_code(struct error *self);
str_t error_get_message(struct error *self);
void error_destroy(struct error *self);

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_ERROR_ERROR_H
