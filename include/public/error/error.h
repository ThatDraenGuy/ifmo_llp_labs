//
// Created by draen on 12.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_ERROR_ERROR_H
#define LLP_LAB_INCLUDE_PUBLIC_ERROR_ERROR_H

#include <stddef.h>
typedef struct {
  size_t bytes;
} error_code_t;

struct error;

struct error *error_new(const char *error_source, const char *error_type,
                        error_code_t error_code, const char *error_message);
void error_destroy(struct error *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_ERROR_ERROR_H
