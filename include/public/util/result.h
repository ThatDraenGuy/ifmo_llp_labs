//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H

#include "public/error/error.h"
#include <stdbool.h>

// typedef enum result { RESULT_OK, RESULT_ERR } result_t;

typedef struct {
  enum { RESULT_OK, RESULT_ERR } type;
  struct error *error;
} result_t;

static inline result_t result_ok() {
  return (result_t){.type = RESULT_OK, .error = NULL};
}

static inline result_t result_err(struct error *error) {
  return (result_t){.type = RESULT_ERR, .error = error};
}

static inline bool result_is_err(result_t result) {
  return result.type == RESULT_ERR;
}

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H
