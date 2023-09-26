//
// Created by draen on 23.09.23.
//

#include "public/error/error_handler.h"
#include "private/error/error.h"
#include <stdio.h>

void handle_error(struct error *err) {
  printf("Encountered error at %s:\n %s(%zu): %s\n", err->error_source,
         err->error_type, err->error_code.bytes, err->error_message);
  error_destroy(err);
}