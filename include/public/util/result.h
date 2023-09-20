//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H

#include "public/error/error.h"
#include <stdbool.h>

typedef struct {
  enum { RESULT_OK, RESULT_ERR } type;
  struct error *error;
} result_t;

#define OK                                                                     \
  return (result_t) { .type = RESULT_OK, .error = NULL }

#define TRY(Action)                                                            \
  {                                                                            \
    result_t __res = Action

#define CATCH(ErrorName, OnErr)                                                \
  if (result_is_err(__res)) {                                                  \
    struct error *ErrorName = __res.error;                                     \
    OnErr;                                                                     \
  }                                                                            \
  }

#define THROW(Error)                                                           \
  return (result_t) { .type = RESULT_ERR, .error = Error }

static inline bool result_is_err(result_t result) {
  return result.type == RESULT_ERR;
}

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_RESULT_H
