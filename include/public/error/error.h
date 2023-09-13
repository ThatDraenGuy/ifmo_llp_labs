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
char *error_get_source(struct error *self);
char *error_get_message(struct error *self);
void error_destroy(struct error *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_ERROR_ERROR_H
