//
// Created by draen on 12.09.23.
//

#include "public/error/errors_common.h"

const char *const common_error_type = "COMMON_ERROR";

const char *const common_error_messages[] = {
    [ERR_COMMON_NULL_POINTER] = "Provided pointer is Null!",
    [ERR_COMMON_ITER_OUT_OF_RANGE] = "Iterator is out of range!"};