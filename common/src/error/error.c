//
// Created by draen on 12.09.23.
//

#include "common/private/error/error.h"
#include <malloc.h>

struct error *error_new(str_t error_source, str_t error_type,
                        error_code_t error_code, str_t error_message) {
  struct error *err = malloc(sizeof(struct error));

  err->error_source = str_into(error_source);
  err->error_type = str_into(error_type);
  err->error_code = error_code;
  err->error_message = str_into(error_message);

  return err;
}

str_t app_error_get_type(struct error *self) {
  return string_as_str(self->error_type);
}

error_code_t error_get_code(struct error *self) { return self->error_code; }

void error_destroy(struct error *self) {
  string_destroy(self->error_source);
  string_destroy(self->error_type);
  string_destroy(self->error_message);
  free(self);
}
