//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_UTIL_HASHMAP_H
#define LLP_LAB_INCLUDE_PRIVATE_UTIL_HASHMAP_H

#include "private//util/linked_list.h"
#include "public/util/hashmap.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_HASH_TABLE_SIZE 101

struct hash_entry {
  void *key;
  void *value;
};

struct hash_entry_list {
  struct linked_list parent;
};

struct hashmap {
  size_t hash_table_size;
  struct hash_entry_list *hash_table;
  uint32_t (*hash_impl)(void *key);
  bool (*keys_equal_impl)(void *key1, void *key2);
  void (*destroy_key_impl)(void *key);
  void (*destroy_value_impl)(void *value);
};

void hashmap_ctor(struct hashmap *hashmap);

#endif // LLP_LAB_INCLUDE_PRIVATE_UTIL_HASHMAP_H
