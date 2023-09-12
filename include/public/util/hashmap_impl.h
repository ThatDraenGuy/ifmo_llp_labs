//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_IMPL_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_IMPL_H

#include <stddef.h>

struct size_size_hashmap;

struct size_size_hashmap *size_size_hashmap_new();
size_t *size_size_hashmap_get(struct size_size_hashmap *hashmap, size_t key);
void size_size_hashmap_insert(struct size_size_hashmap *hashmap, size_t key,
                              size_t value);

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_IMPL_H
