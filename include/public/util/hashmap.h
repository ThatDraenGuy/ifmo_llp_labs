//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_H

#include <stddef.h>

struct hashmap;

void *hashmap_get(struct hashmap *hashmap, void *key);
void hashmap_insert(struct hashmap *hashmap, void *key, void *value);

void hashmap_destroy(struct hashmap *hashmap);

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_HASHMAP_H
