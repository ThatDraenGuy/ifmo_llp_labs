//
// Created by draen on 10.09.23.
//

#include "private/storage/page_group_manager.h"
#include "public/error/errors_common.h"
#include "public/file/page_manager.h"
#include "public/file/page_resolver.h"
#include <malloc.h>

#define ERROR_SOURCE "PAGE_GROUP_MANAGER"
#define ITERATOR_ERROR_SOURCE "PAGE_ITERATOR"

bool page_iterator_has_next(struct page_iterator *self) {
  return !page_id_is_null(self->next_page_id);
}
result_t page_iterator_current(struct page_iterator *self, page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  page_t page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, self->current_page_id, &page));
  CATCH(error, THROW(error))

  // skip page_header for the resulting page
  result->data = page.data + sizeof(struct page_header);
  OK;
}
result_t page_iterator_next(struct page_iterator *self, page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (!page_iterator_has_next(self))
    THROW(error_common(ITERATOR_ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  page_t page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, self->next_page_id, &page));
  CATCH(error, THROW(error))

  // skip page_header for the resulting page
  result->data = page.data + sizeof(struct page_header);

  struct page_header *page_header = page.data;
  self->current_page_id = self->next_page_id;
  self->next_page_id = page_header->next;
  OK;
}

void page_iterator_destroy(struct page_iterator *self) { free(self); }

static struct page_iterator *
page_iterator_new(struct page_manager *page_manager,
                  page_group_id_t page_group_id) {
  struct page_iterator *it = malloc(sizeof(struct page_iterator));
  it->page_manager = page_manager;
  it->current_page_id = PAGE_ID_NULL;
  it->next_page_id = (page_id_t){page_group_id.bytes};
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
  TRY(get_new_page(self, result, result_id));
  CATCH(error, THROW(error))

  struct page_header *page_header = result->data;
  page_header->next = PAGE_ID_NULL;

  OK;
}

struct page_group_manager *page_group_manager_new() {
  return malloc(sizeof(struct page_group_manager));
}

result_t page_group_manager_ctor(struct page_group_manager *self,
                                 char *file_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct application_header *default_header =
      malloc(sizeof(struct application_header));
  default_header->first_free_page = PAGE_ID_NULL;
  default_header->last_free_page = PAGE_ID_NULL;
  default_header->meta_page = PAGE_ID_NULL;

  struct page_resolver *page_resolver = page_resolver_new();
  TRY(page_resolver_ctor(page_resolver, file_name,
                         sizeof(struct application_header), default_header));
  CATCH(error, {
    free(default_header);
    free(self);
    THROW(error);
  })

  free(default_header);

  struct page_manager *page_manager = page_manager_new();
  TRY(page_manager_ctor(page_manager, page_resolver, 10));
  CATCH(error, {
    free(self);
    THROW(error);
  })

  self->page_manager = page_manager;
  OK;
}

page_group_id_t
page_group_manager_get_meta_group_id(struct page_group_manager *self) {
  struct application_header *header =
      page_manager_get_application_header(self->page_manager);
  return (page_group_id_t){header->meta_page.bytes};
}

result_t page_group_manager_set_meta_group_id(struct page_group_manager *self,
                                              page_group_id_t page_group_id) {
  struct application_header *header =
      page_manager_get_application_header(self->page_manager);
  header->meta_page = (page_id_t){page_group_id.bytes};
  return page_manager_flush_application_header(self->page_manager);
}

result_t page_group_manager_add_page(struct page_group_manager *self,
                                     struct page_iterator *it, page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  page_id_t new_last_page_id = PAGE_ID_NULL;
  page_t new_page = PAGE_NULL;
  TRY(create_page(self, &new_page, &new_last_page_id));
  CATCH(error, THROW(error))
  result->data = new_page.data + sizeof(struct page_header);

  page_t old_last_page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, it->current_page_id,
                            &old_last_page));
  CATCH(error, THROW(error))

  struct page_header *old_page_header = old_last_page.data;
  struct page_header *new_page_header = new_page.data;
  if (page_iterator_has_next(it)) {
    new_page_header->next = old_page_header->next;
  } else {
    new_page_header->next = PAGE_ID_NULL;
  }
  old_page_header->next = new_last_page_id;

  OK;
}

result_t page_group_manager_create_group(struct page_group_manager *self,
                                         page_group_id_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  page_t new_page = PAGE_NULL;
  page_id_t new_page_id = PAGE_ID_NULL;
  TRY(create_page(self, &new_page, &new_page_id));
  CATCH(error, THROW(error))

  *result = (page_group_id_t){new_page_id.bytes};
  OK;
}

result_t page_group_manager_delete_group(struct page_group_manager *self,
                                         page_group_id_t page_group_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  page_id_t to_be_freed_page_id = (page_id_t){page_group_id.bytes};

  struct application_header *application_header =
      page_manager_get_application_header(self->page_manager);

  page_t page = PAGE_NULL;
  struct page_header *header = NULL;
  if (page_id_is_null(application_header->last_free_page)) {
    // initialize first_free_page and setup page & header to find last_free_page
    application_header->first_free_page = to_be_freed_page_id;

    TRY(page_manager_get_page(self->page_manager, to_be_freed_page_id, &page));
    CATCH(error, THROW(error))

    header = page.data;
  } else {
    // update next page in free page chain
    TRY(page_manager_get_page(self->page_manager,
                              application_header->last_free_page, &page));
    CATCH(error, THROW(error))

    header = page.data;
    header->next = to_be_freed_page_id;
  }

  // update last_free_page field
  page_id_t last_page_id = to_be_freed_page_id;
  while (!page_id_is_null(header->next)) {
    last_page_id = header->next;
    TRY(page_manager_get_page(self->page_manager, header->next, &page));
    CATCH(error, THROW(error))

    header = page.data;

    application_header->last_free_page = last_page_id;
  }

  return page_manager_flush_application_header(self->page_manager);
}

size_t page_group_manager_get_page_capacity(struct page_group_manager *self) {
  return page_manager_get_page_size(self->page_manager) -
         sizeof(struct page_header);
}

result_t page_group_manager_flush(struct page_group_manager *self) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
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