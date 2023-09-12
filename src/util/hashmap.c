//
// Created by draen on 08.09.23.
//

#include "private/util/hashmap.h"
#include <malloc.h>

void hash_entry_destroy(void *data) { struct hash_entry *entry = data; }

void hash_entry_list_ctor(struct hash_entry_list *list) {
  linked_list_ctor((struct linked_list *)list);
  list->parent.destroy_node_data_impl = hash_entry_destroy;
}

static uint32_t hash(struct hashmap *hashmap, void *key) {
  return hashmap->hash_impl(key) % hashmap->hash_table_size;
}

static struct hash_entry *lookup(struct hashmap *hashmap, void *key) {
  struct hash_entry_list *entry_list = &hashmap->hash_table[hash(hashmap, key)];

  struct i_iterator *it = list_iterator((struct linked_list *)entry_list);

  while (iterator_has_next(it)) {
    struct hash_entry *entry = iterator_next(it);
    if (hashmap->keys_equal_impl(key, entry->key))
      return entry;
  }
  return NULL;
}

void hashmap_ctor(struct hashmap *hashmap) {
  hashmap->hash_table_size = DEFAULT_HASH_TABLE_SIZE;
  hashmap->hash_table =
      malloc(hashmap->hash_table_size * sizeof(struct hash_entry_list));
}

void *hashmap_get(struct hashmap *hashmap, void *key) {
  struct hash_entry *entry = lookup(hashmap, key);
  if (entry == NULL)
    return NULL;
  return entry->value;
}

void hashmap_insert(struct hashmap *hashmap, void *key, void *value) {

  struct hash_entry *old_entry = lookup(hashmap, key);

  if (old_entry == NULL) {
    struct hash_entry *new_entry = malloc(sizeof(struct hash_entry));
    new_entry->key = key;
    new_entry->value = value;
    struct hash_entry_list *entry_list =
        &hashmap->hash_table[hash(hashmap, key)];
    linked_list_ctor((struct linked_list *)entry_list);
    linked_list_push_back((struct linked_list *)entry_list, new_entry);

    //    hashmap->hash_table[hash(hashmap, key)] = new_entry;
  } else {
    hashmap->destroy_key_impl(old_entry->key);
    hashmap->destroy_value_impl(old_entry->value);
    old_entry->key = key;
    old_entry->value = value;
  }
}

void hashmap_destroy(struct hashmap *hashmap) {
  for (size_t i = 0; i < hashmap->hash_table_size; i++) {
    struct hash_entry_list *entry_list = &hashmap->hash_table[i];
    linked_list_destroy((struct linked_list *)entry_list);
    //    struct i_iterator *it = list_iterator((struct linked_list
    //    *)entry_list); while (iterator_has_next(it)) {
    //      struct hash_entry *entry = iterator_next(it);
    //      hashmap->destroy_key_impl(entry->key);
    //      hashmap->destroy_value_impl(entry->value);
    //    }
  }
  free(hashmap->hash_table);
  free(hashmap);
}