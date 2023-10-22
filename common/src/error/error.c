//
// Created by draen on 12.09.23.
//

#include "common/private/error/error.h"
#include <malloc.h>
#include <string.h>

struct error *error_new(const char *const error_source,
                        const char *const error_type, error_code_t error_code,
                        const char *const error_message) {
  struct error *err = malloc(sizeof(struct error));

  //  char *actual_error_source = malloc(strlen(ERROR_SOURCE));
  //  char *actual_error_type = malloc(strlen(ERROR_TYPE));
  //  char *actual_error_message = malloc(strlen(error_message));
  //  err->ERROR_SOURCE = actual_error_source;
  //  err->ERROR_TYPE = actual_error_type;
  //  err->error_code = error_code;
  //  err->error_message = actual_error_message;

  err->error_source = error_source;
  err->error_type = error_type;
  err->error_code = error_code;
  err->error_message = error_message;

  return err;
}

char *error_get_type(struct error *self) { return self->error_type; }

error_code_t error_get_code(struct error *self) { return self->error_code; }

void error_destroy(struct error *self) {
  //  free(self->ERROR_SOURCE);
  //  free(self->ERROR_TYPE);
  //  free(self->error_message);
  free(self);
}
