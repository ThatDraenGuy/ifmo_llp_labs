//
// Created by draen on 12.09.23.
//

#include "common/public/error/errors_common.h"

const str_t common_error_type = STR_OF("COMMON_ERROR");

const str_t common_error_messages[] = {
    [ERR_COMMON_NULL_POINTER] = STR_OF("Provided pointer is Null!"),
    [ERR_COMMON_ITER_OUT_OF_RANGE] = STR_OF("Iterator is out of range!"),
    [ERR_INDEX_OUT_OF_BOUNDS] = STR_OF("Index is out of bounds!")};