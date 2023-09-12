//
// Created by draen on 08.09.23.
//

#include "private/util/hashmap_impl.h"
#include <malloc.h>

static uint32_t hash(void *key) {
  size_t *size_key = key;

  return *size_key; // TODO
}

static bool keys_equal(void *key1, void *key2) {
  size_t *size_key1 = key1;
  size_t *size_key2 = key2;
  return *size_key1 == *size_key2;
}

static void destroy_size(void *data) { free(data); }

struct size_size_hashmap *size_size_hashmap_new() {
  struct size_size_hashmap *hashmap = malloc(sizeof(struct size_size_hashmap));
  hashmap_ctor((struct hashmap *)hashmap);

  hashmap->parent.hash_impl = hash;
  hashmap->parent.keys_equal_impl = keys_equal;
  hashmap->parent.destroy_key_impl = destroy_size;
  hashmap->parent.destroy_value_impl = destroy_size;

  return hashmap;
}

size_t *size_size_hashmap_get(struct size_size_hashmap *hashmap, size_t key) {
  return hashmap_get((struct hashmap *)hashmap, &key);
}

void size_size_hashmap_insert(struct size_size_hashmap *hashmap, size_t key,
                              size_t value) {
  size_t *stored_key = malloc(sizeof(size_t));
  *stored_key = key;

  size_t *stored_value = malloc(sizeof(size_t));
  *stored_value = value;
  hashmap_insert((struct hashmap *)hashmap, stored_key, stored_value);
}