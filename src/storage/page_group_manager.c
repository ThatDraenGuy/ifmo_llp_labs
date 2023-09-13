//
// Created by draen on 10.09.23.
//

#include "private/storage/page_group_manager.h"
#include "public/error/errors_common.h"
#include "public/file/cached_page_manager.h"
#include "public/file/file_page_resolver.h"
#include <malloc.h>

static const char *const error_source = "PAGE_GROUP_MANAGER";
static const char *const iterator_error_source = "PAGE_ITERATOR";

bool page_iterator_has_next(struct page_iterator *self) {
  return !page_id_is_null(self->next_page_id);

  //  struct page_header *page_header = self->current_page.data;
  //  return page_id_is_null(page_header->next);
}

result_t page_iterator_next(struct page_iterator *self, page_t *result) {
  result_t res;

  if (!page_iterator_has_next(self))
    return result_err(
        error_common(iterator_error_source, ERR_COMMON_ITER_OUT_OF_RANGE));

  res = page_manager_get_page(self->page_manager, self->next_page_id, result);
  if (result_is_err(res)) {
    return res;
  }
  self->current_page = *result;
  struct page_header *page_header = self->current_page.data;
  self->next_page_id = page_header->next;
  return result_ok();
}

void page_iterator_destroy(struct page_iterator *self) { free(self); }

static struct page_iterator *
page_iterator_new(struct i_page_manager *page_manager,
                  page_group_id_t page_group_id) {
  struct page_iterator *it = malloc(sizeof(struct page_iterator));
  it->page_manager = page_manager;
  it->current_page = PAGE_NULL;
  it->next_page_id = (page_id_t){page_group_id.bytes};

  //  page_manager_get_page(page_manager, (page_id_t){page_group_id.bytes},
  //                        &it->current_page);

  return it;
}

static result_t get_new_page(struct page_group_manager *self, page_t *result,
                             page_id_t *result_id) {
  struct application_header *header =
      page_manager_get_application_header(self->page_manager);

  if (page_id_is_null(header->first_free_page))
    return page_manager_create_page(self->page_manager, result, result_id);

  // TODO alter first_free_page
  *result_id = header->first_free_page;
  return page_manager_get_page(self->page_manager, *result_id, result);
}

static result_t create_page(struct page_group_manager *self, page_t *result,
                            page_id_t *result_id) {
  result_t res;

  res = get_new_page(self, result, result_id);
  if (result_is_err(res))
    return res;

  struct page_header *page_header = result->data;
  page_header->next = PAGE_ID_NULL;
  page_header->is_free = false;

  return result_ok();
}

struct page_group_manager *page_group_manager_new() {
  return malloc(sizeof(struct page_group_manager));
}

result_t page_group_manager_ctor(struct page_group_manager *self,
                                 char *file_name) {
  result_t res;

  struct application_header *default_header =
      malloc(sizeof(struct application_header));
  default_header->first_free_page = PAGE_ID_NULL;
  default_header->last_free_page = PAGE_ID_NULL;
  default_header->meta_page = PAGE_ID_NULL;

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
  res = create_page(self, result, &page_id);
  if (result_is_err(res)) {
    return res;
  }

  struct page_header *page_header = it->current_page.data;
  struct page_header *new_page_header = result->data;
  if (page_iterator_has_next(it)) {
    new_page_header->next = page_header->next;
  }
  page_header->next = page_id;

  return result_ok();
}

result_t page_group_manager_create_group(struct page_group_manager *self,
                                         page_group_id_t *result) {
  result_t res;
  page_t new_page = PAGE_NULL;
  page_id_t new_page_id = PAGE_ID_NULL;

  res = create_page(self, &new_page, &new_page_id);
  if (result_is_err(res))
    return res;

  *result = (page_group_id_t){new_page_id.bytes};
  return result_ok();
}

size_t page_group_manager_get_page_size(struct page_group_manager *self) {
  return page_manager_get_page_size(self->page_manager) -
         sizeof(struct page_header);
}

result_t page_group_manager_flush(struct page_group_manager *self) {
  return page_manager_flush(self->page_manager);
}
void page_group_manager_destroy(struct page_group_manager *self) {
  page_manager_destroy(self->page_manager);
  free(self);
}

struct page_iterator *
page_group_manager_get_group(struct page_group_manager *self,
                             page_group_id_t page_group_id) {
  return page_iterator_new(self->page_manager, page_group_id);
}