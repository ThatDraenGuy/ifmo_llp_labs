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
  return (struct cache_entry *)(cached_page_manager->cache + offset);
}

static void cache_new(struct cached_page_manager *cached_page_manager) {
  size_t entry_size = cache_entry_size(cached_page_manager);

  size_t actual_size = cached_page_manager->cache_size.bytes * entry_size;
  cached_page_manager->cache = malloc(actual_size);

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < cached_page_manager->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(cached_page_manager, index);
    entry->page_id = PAGE_ID_NULL;
    entry->relevancy_value = LEAST_RELEVANT_VALUE;
    entry->is_altered = false;
  }
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
    if (entry->relevancy_value.bytes <
        least_relevant_entry->relevancy_value.bytes)
      least_relevant_entry = entry;
  }
  return least_relevant_entry;
}

static result_t flush_entry(struct cached_page_manager *cached_page_manager,
                            struct cache_entry *entry) {
  if (!entry->is_altered)
    return result_ok();

  return page_resolver_write_page(cached_page_manager->page_resolver,
                                  entry->page_id, (page_t){entry->contents});
}

static result_t load_page_from_resolver(struct cached_page_manager *self,
                                        page_id_t page_id, page_t *result) {
  result_t res;

  struct cache_entry *entry = get_least_relevant_entry(self);
  res = flush_entry(self, entry);
  if (result_is_err(res))
    return res;

  result->data = entry->contents;
  entry->relevancy_value = MOST_RELEVANT_VALUE;
  return page_resolver_read_page(self->page_resolver, page_id, *result);
}

static void
load_page_from_cache(struct cached_page_manager *cached_page_manager,
                     cache_entry_index_t cache_index, page_t *result) {
  struct cache_entry *entry = get_cache_entry(cached_page_manager, cache_index);
  entry->is_altered = true;
  result->data = entry->contents;
}

static result_t create_page(void *cached_page_manager, page_t *result,
                            page_id_t *result_id) {
  result_t res;

  struct cached_page_manager *self = cached_page_manager;
  struct cache_entry *entry = get_least_relevant_entry(self);
  res = flush_entry(self, entry);
  if (result_is_err(res))
    return res;

  entry->relevancy_value = MOST_RELEVANT_VALUE;
  entry->is_altered = true;
  res = page_resolver_get_new_page_id(self->page_resolver, &entry->page_id);
  if (result_is_err(res))
    return res;

  *result_id = entry->page_id;

  *result = (page_t){entry->contents};

  return result_ok();
}

static void *get_application_header(void *self) {
  struct cached_page_manager *cached_page_manager = self;
  return page_resolver_get_application_header(
      cached_page_manager->page_resolver);
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
    if (entry->relevancy_value.bytes != 0 &&
        entry->page_id.bytes == page_id.bytes) {
      load_page_from_cache(cached_page_manager, index, result);
      return result_ok();
    }
  }
  return load_page_from_resolver(cached_page_manager, page_id, result);
}

static result_t flush(void *cached_page_manager) {
  result_t res;
  struct cached_page_manager *self = cached_page_manager;

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    if (entry->is_altered) {
      res = page_resolver_write_page(self->page_resolver, entry->page_id,
                                     (page_t){entry->contents});
      if (result_is_err(res))
        return res;

      entry->relevancy_value = LEAST_RELEVANT_VALUE; // TODO think
      entry->is_altered = false;
    }
  }

  return result_ok();
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

result_t cached_page_manager_ctor(struct cached_page_manager *self,
                                  struct i_page_resolver *page_resolver,
                                  size_t cache_size) {
  self->page_resolver = page_resolver;
  self->cache_size = (cache_entry_index_t){.bytes = cache_size};

  self->parent.create_page_impl = create_page;
  self->parent.get_application_header_impl = get_application_header;
  self->parent.get_page_size_impl = get_page_size;
  self->parent.get_page_impl = get_page;
  self->parent.flush_impl = flush;
  self->parent.destroy_impl = destroy;

  cache_new(self);

  return result_ok();
}