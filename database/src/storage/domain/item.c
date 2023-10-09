//
// Created by draen on 20.09.23.
//

#include "public/storage/domain/item.h"
#include <malloc.h>

void item_destroy(item_t item) { free(item.data); }