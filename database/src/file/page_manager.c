//
// Created by draen on 08.09.23.
//

#include "database/private/file/page_manager.h"
#include "common/public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

#define ERROR_SOURCE STR_OF("PAGE_RESOLVER")

static size_t cache_entry_size(struct page_manager *self) {
  size_t page_size = page_manager_get_page_size(self);
  return sizeof(struct cache_entry) + page_size;
}
static struct cache_entry *get_cache_entry(struct page_manager *self,
                                           cache_entry_index_t index) {
  size_t entry_size = cache_entry_size(self);
  size_t offset = entry_size * index.bytes;
  return (struct cache_entry *)(self->cache + offset);
}

static void cache_new(struct page_manager *self) {
  size_t entry_size = cache_entry_size(self);

  size_t actual_size = self->cache_size.bytes * entry_size;
  self->cache = malloc(actual_size);

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    entry->page_id = PAGE_ID_NULL;
    entry->relevancy_value = LEAST_RELEVANT_VALUE;
    entry->is_altered = false;
  }
}

static void cache_destroy(struct page_manager *self) { free(self->cache); }

static struct cache_entry *get_least_relevant_entry(struct page_manager *self) {
  struct cache_entry *least_relevant_entry =
      get_cache_entry(self, (cache_entry_index_t){.bytes = 0});
  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 1};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    if (entry->relevancy_value.bytes <
        least_relevant_entry->relevancy_value.bytes)
      least_relevant_entry = entry;
  }
  return least_relevant_entry;
}

static result_t flush_entry(struct page_manager *self,
                            struct cache_entry *entry) {
  if (!entry->is_altered)
    OK;

  return page_resolver_write_page(self->page_resolver, entry->page_id,
                                  (page_t){entry->contents});
}

static void reset_relevancy_values(struct page_manager *self) {
  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    entry->relevancy_value.bytes = 0;
  }
  self->current_most_relevant_value.bytes = 1;
}

static relevancy_value_t get_relevancy_value(struct page_manager *self,
                                             page_id_t page_id) {
  if (self->current_most_relevant_value.bytes == RELEVANCY_VALUE_MAX.bytes) {
    reset_relevancy_values(self);
  }
  if (self->last_accessed_page.bytes == page_id.bytes) {
    return self->current_most_relevant_value;
  } else {
    self->current_most_relevant_value.bytes++;
    self->last_accessed_page = page_id;
    return self->current_most_relevant_value;
  }
}

static result_t load_page_from_resolver(struct page_manager *self,
                                        page_id_t page_id, page_t *result) {

  struct cache_entry *entry = get_least_relevant_entry(self);
  TRY(flush_entry(self, entry));
  CATCH(error, THROW(error))

  result->data = entry->contents;
  entry->relevancy_value = get_relevancy_value(self, page_id);
  entry->page_id = page_id;
  return page_resolver_read_page(self->page_resolver, page_id, *result);
}

static void load_page_from_cache(struct page_manager *self,
                                 cache_entry_index_t cache_index,
                                 page_id_t page_id, page_t *result) {
  struct cache_entry *entry = get_cache_entry(self, cache_index);
  entry->is_altered = true;
  entry->relevancy_value = get_relevancy_value(self, page_id);
  result->data = entry->contents;
}

void *page_manager_get_application_header(struct page_manager *self) {
  return page_resolver_get_application_header(self->page_resolver);
}

result_t page_manager_flush_application_header(struct page_manager *self) {
  return page_resolver_flush_application_header(self->page_resolver);
}

result_t page_manager_create_page(struct page_manager *self, page_t *result,
                                  page_id_t *result_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct cache_entry *entry = get_least_relevant_entry(self);
  TRY(flush_entry(self, entry));
  CATCH(error, THROW(error))

  entry->relevancy_value = get_relevancy_value(self, PAGE_ID_NULL);
  entry->is_altered = true;
  memset(entry->contents, '\0', page_manager_get_page_size(self));
  TRY(page_resolver_get_new_page_id(self->page_resolver, &entry->page_id));
  CATCH(error, THROW(error))

  *result_id = entry->page_id;

  *result = (page_t){entry->contents};

  OK;
}

result_t page_manager_get_page(struct page_manager *self, page_id_t page_id,
                               page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    if (entry->relevancy_value.bytes != 0 &&
        entry->page_id.bytes == page_id.bytes) {
      self->cache_count++;
      load_page_from_cache(self, index, page_id, result);
      OK;
    }
  }
  self->non_cache_count++;
  return load_page_from_resolver(self, page_id, result);
}

size_t page_manager_get_page_size(struct page_manager *self) {
  return page_resolver_get_page_size(self->page_resolver);
}

result_t page_manager_flush(struct page_manager *self) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  for (cache_entry_index_t index = (cache_entry_index_t){.bytes = 0};
       index.bytes < self->cache_size.bytes; index.bytes++) {
    struct cache_entry *entry = get_cache_entry(self, index);
    if (entry->is_altered) {
      TRY(page_resolver_write_page(self->page_resolver, entry->page_id,
                                   (page_t){entry->contents}));
      CATCH(error, THROW(error))

      entry->relevancy_value = LEAST_RELEVANT_VALUE;
      entry->is_altered = false;
    }
  }

  OK;
}

void page_manager_destroy(struct page_manager *self) {
  page_manager_flush(self);
  page_resolver_destroy(self->page_resolver);
  cache_destroy(self);
  free(self);
}

struct page_manager *page_manager_new() {
  return malloc(sizeof(struct page_manager));
}

result_t page_manager_ctor(struct page_manager *self,
                           struct page_resolver *page_resolver,
                           size_t cache_size) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  self->cache_count = 0;
  self->non_cache_count = 0;
  self->page_resolver = page_resolver;
  self->cache_size = (cache_entry_index_t){.bytes = cache_size};
  self->current_most_relevant_value = (relevancy_value_t){1};
  self->last_accessed_page = PAGE_ID_NULL;

  cache_new(self);

  OK;
}