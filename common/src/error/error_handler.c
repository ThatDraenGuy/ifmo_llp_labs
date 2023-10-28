//
// Created by draen on 23.09.23.
//

#include "common/public/error/error_handler.h"
#include "common/private/error/error.h"
#include <stdio.h>

void handle_error(struct error *err) {
  printf("Encountered error at %s:\n %s(%zu): %s\n", err->error_source._data,
         err->error_type._data, err->error_code.bytes,
         err->error_message._data);
  error_destroy(err);
}