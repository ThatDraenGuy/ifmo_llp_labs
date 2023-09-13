//
// Created by draen on 12.09.23.
//

#include "private/error/error.h"
#include <malloc.h>
#include <string.h>

struct error *error_new(const char *const error_source,
                        const char *const error_type, error_code_t error_code,
                        const char *const error_message) {
  struct error *err = malloc(sizeof(struct error));

  //  char *actual_error_source = malloc(strlen(error_source));
  //  char *actual_error_type = malloc(strlen(error_type));
  //  char *actual_error_message = malloc(strlen(error_message));
  //  err->error_source = actual_error_source;
  //  err->error_type = actual_error_type;
  //  err->error_code = error_code;
  //  err->error_message = actual_error_message;

  err->error_source = error_source;
  err->error_type = error_type;
  err->error_code = error_code;
  err->error_message = error_message;

  return err;
}

void error_destroy(struct error *self) {
  //  free(self->error_source);
  //  free(self->error_type);
  //  free(self->error_message);
  free(self);
}
