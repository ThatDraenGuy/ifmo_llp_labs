//
// Created by draen on 10.09.23.
//

#include "private/storage/page_group_manager.h"
#include "public/error/errors_common.h"
#include "public/file/cached_page_manager.h"
#include "public/file/file_page_resolver.h"
#include <malloc.h>

const char *const error_source = "PAGE_GROUP_MANAGER";
const char *const iterator_error_source = "PAGE_ITERATOR";

bool page_iterator_has_next(struct page_iterator *self) {
  if (page_is_null(self->current_page))
    return false;

  struct page_header *page_header = self->current_page.data;
  return page_id_is_null(page_header->next);
}

result_t page_iterator_next(struct page_iterator *self, page_t *result) {
  result_t res;

  if (!page_iterator_has_next(self))
    return result_err(
        error_common(iterator_error_source, ERR_COMMON_ITER_OUT_OF_RANGE));
  struct page_header *page_header = self->current_page.data;
  //  page_t next_page = PAGE_NULL;
  res = page_manager_get_page(self->page_manager, page_header->next, result);
  if (result_is_err(res)) {
    return res;
  }
  self->current_page = *result;
  return result_ok();
}

void page_iterator_destroy(struct page_iterator *self) { free(self); }

static struct page_iterator *
page_iterator_new(struct i_page_manager *page_manager,
                  page_group_id_t page_group_id) {
  struct page_iterator *it = malloc(sizeof(struct page_iterator));
  it->page_manager = page_manager;

  page_manager_get_page(page_manager, (page_id_t){page_group_id.bytes},
                        &it->current_page);

  return it;
}

struct page_group_manager *page_group_manager_new() {
  return malloc(sizeof(struct page_group_manager));
}

result_t page_group_manager_ctor(struct page_group_manager *self,
                                 char *file_name) {
  result_t res;

  struct application_header *default_header =
      malloc(sizeof(struct application_header));
  default_header->first_free_page = 0;
  default_header->last_free_page = 0;
  default_header->meta_page = 0;

  struct file_page_resolver *page_resolver = file_page_resolver_new();
  res = file_page_resolver_ctor(page_resolver, file_name,
                                sizeof(struct application_header),
                                default_header);
  if (result_is_err(res)) {
    free(default_header);
    free(self);
    return res;
  }

  free(default_header);

  struct cached_page_manager *page_manager = cached_page_manager_new();
  res = cached_page_manager_ctor(page_manager,
                                 (struct i_page_resolver *)page_resolver, 10);
  if (result_is_err(res)) {
    free(self);
    return res;
  }

  self->page_manager = (struct i_page_manager *)page_manager;

  return result_ok();
}

result_t page_group_manager_add_page(struct page_group_manager *self,
                                     struct page_iterator *it, page_t *result) {
  result_t res;

  page_id_t page_id = PAGE_ID_NULL;
  res = page_manager_create_page(self->page_manager, result, &page_id);
  if (result_is_err(res)) {
    return res;
  }

  if (page_iterator_has_next(it)) {

  } else {
  }
}

struct page_iterator *
page_group_manager_get_group(struct page_group_manager *self,
                             page_group_id_t page_group_id) {
  return page_iterator_new(self->page_manager, page_group_id);
}