//
// Created by draen on 08.09.23.
//

#include "private/file/cached_page_manager.h"
#include <malloc.h>

static size_t
cache_entry_size(struct cached_page_manager *cached_page_manager) {
  size_t page_size =
      page_manager_get_page_size((struct i_page_manager *)cached_page_manager);
  return sizeof(struct cache_entry) + page_size;
}
static struct cache_entry *
get_cache_entry(struct cached_page_manager *cached_page_manager,
                cache_entry_index_t index) {
  size_t entry_size = cache_entry_size(cached_page_manager);
  size_t offset = entry_size * index.bytes;
  return cached_page_manager->cache + offset;
}

static void cache_new(struct cached_page_manager *cached_page_manager) {
  size_t entry_size = cache_entry_size(cached_page_manager);

  size_t actual_size = cached_page_manager->cache_size.bytes * entry_size;
  cached_page_manager->cache = malloc(actual_size);
}

static void cache_destroy(struct cached_page_manager *cached_page_manager) {
  free(cached_page_manager->cache);
}

static struct cache_entry *
get_least_relevant_entry(struct cached_page_manager *cached_page_manager) {
  struct cache_entry *least_relevant_entry =
      get_cache_entry(cached_page_manager, (cache_entry_index_t){.bytes = 0});
  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 1};
       index.bytes < cached_page_manager->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(cached_page_manager, index);
    if (entry->relevancy_value < least_relevant_entry->relevancy_value)
      least_relevant_entry = entry;
  }
  return least_relevant_entry;
}

static result_t load_page(struct cached_page_manager *cached_page_manager,
                          page_id_t page_id, page_t *result) {
  struct cache_entry *entry = get_least_relevant_entry(cached_page_manager);
  result->data = entry->contents;
  return page_resolver_read_page(cached_page_manager->page_resolver, page_id,
                                 *result);
}

static void get_page_from_cache(struct cached_page_manager *cached_page_manager,
                                cache_entry_index_t cache_index,
                                page_t *result) {
  result->data = get_cache_entry(cached_page_manager, cache_index)->contents;
}

static size_t get_page_size(void *this) {
  struct cached_page_manager *cached_page_manager = this;
  return page_resolver_get_page_size(
      (struct i_page_resolver *)cached_page_manager->page_resolver);
}

static result_t get_page(void *this, page_id_t page_id, page_t *result) {
  struct cached_page_manager *cached_page_manager = this;

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < cached_page_manager->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(cached_page_manager, index);
    if (entry->relevancy_value != 0 && entry->page_id.bytes == page_id.bytes) {
      get_page_from_cache(cached_page_manager, index, result);
      return RESULT_OK;
    }
  }
  return load_page(cached_page_manager, page_id, result);
}

static void destroy(void *this) {
  struct cached_page_manager *cached_page_manager = this;
  page_resolver_destroy(
      (struct i_page_resolver *)cached_page_manager->page_resolver);
  cache_destroy(cached_page_manager);
  free(cached_page_manager);
}

struct cached_page_manager *cached_page_manager_new() {
  return malloc(sizeof(struct cached_page_manager));
}

result_t
cached_page_manager_ctor(struct cached_page_manager *cached_page_manager,
                         struct i_page_resolver *page_resolver,
                         size_t cache_size) {
  cached_page_manager->page_resolver = page_resolver;
  cached_page_manager->cache_size = (cache_entry_index_t){.bytes = cache_size};

  cached_page_manager->parent.get_page_size_impl = get_page_size;
  cached_page_manager->parent.get_page_impl = get_page;
  cached_page_manager->parent.destroy_impl = destroy;

  cache_new(cached_page_manager);

  return RESULT_OK;
}