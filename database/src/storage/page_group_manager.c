//
// Created by draen on 10.09.23.
//

#include "database/private/storage/page_group_manager.h"
#include "common/public/error/errors_common.h"
#include "database/public/file/page_manager.h"
#include "database/public/file/page_resolver.h"
#include <malloc.h>

#define ERROR_SOURCE "PAGE_GROUP_MANAGER"
#define ITERATOR_ERROR_SOURCE "PAGE_ITERATOR"

bool page_iterator_has_next(struct page_iterator *self) {
  return self->inverse && (!self->started || self->next_page_id.bytes !=
                                                 self->page_group_id.bytes) ||
         !self->inverse && !page_id_is_null(self->next_page_id);
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

page_id_t page_iterator_current_id(struct page_iterator *self) {
  return self->current_page_id;
}

result_t page_iterator_next(struct page_iterator *self, page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (!page_iterator_has_next(self))
    THROW(error_common(ITERATOR_ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  self->started = true;
  page_t page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, self->next_page_id, &page));
  CATCH(error, THROW(error))

  // skip page_header for the resulting page
  result->data = page.data + sizeof(struct page_header);

  struct page_header *page_header = page.data;
  self->current_page_id = self->next_page_id;
  self->next_page_id =
      self->inverse ? page_header->previous : page_header->next;
  OK;
}

void page_iterator_destroy(struct page_iterator *self) { free(self); }

static struct page_iterator *
page_iterator_new(struct page_manager *page_manager,
                  page_group_id_t page_group_id, bool inverse) {
  struct page_iterator *it = malloc(sizeof(struct page_iterator));
  it->page_manager = page_manager;
  it->inverse = inverse;
  it->started = false;
  it->page_group_id = page_group_id;
  it->current_page_id = PAGE_ID_NULL;
  it->next_page_id = (page_id_t){page_group_id.bytes};
  return it;
}

static result_t get_new_page(struct page_group_manager *self, page_t *result,
                             page_id_t *result_id) {
  struct application_header *app_header =
      page_manager_get_application_header(self->page_manager);

  // if no free pages - create new page and return it
  if (page_id_is_null(app_header->first_free_page))
    return page_manager_create_page(self->page_manager, result, result_id);

  // otherwise get first free page
  *result_id = app_header->first_free_page;
  TRY(page_manager_get_page(self->page_manager, *result_id, result));
  CATCH(error, THROW(error))

  struct page_header *header = result->data;

  // alter first_free_page pointer
  app_header->first_free_page = header->next;
  if (page_id_is_null(app_header->first_free_page)) {
    // if first_free_page is now NULL (no more free pages)
    // set to null last_free_page
    app_header->last_free_page = PAGE_ID_NULL;
  } else {
    // otherwise set to null `previous` pointer in new first free page
    page_t new_first_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, app_header->first_free_page,
                              &new_first_page));
    CATCH(error, THROW(error))
    struct page_header *new_first_header = new_first_page.data;
    new_first_header->previous = PAGE_ID_NULL;
  }

  return page_manager_flush_application_header(self->page_manager);
}

static result_t create_page(struct page_group_manager *self, page_t *result,
                            page_id_t *result_id) {
  TRY(get_new_page(self, result, result_id));
  CATCH(error, THROW(error))

  struct page_header *page_header = result->data;
  page_header->previous = PAGE_ID_NULL;
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
  TRY(page_manager_ctor(page_manager, page_resolver, DEFAULT_CACHE_SIZE));
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
                                     page_group_id_t page_group_id,
                                     page_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  page_id_t new_last_page_id = PAGE_ID_NULL;
  {
    // create new page and set result
    page_t new_page = PAGE_NULL;
    TRY(create_page(self, &new_page, &new_last_page_id));
    CATCH(error, THROW(error))
    result->data = new_page.data + sizeof(struct page_header);
  }

  page_id_t old_last_page_id;
  {
    // get old last page id and update `previous` field in first page
    page_t first_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager,
                              (page_id_t){page_group_id.bytes}, &first_page));
    CATCH(error, THROW(error))
    struct page_header *first_page_header = first_page.data;
    old_last_page_id = first_page_header->previous;
    first_page_header->previous = new_last_page_id;
  }

  {
    // setup newly created page
    page_t new_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, new_last_page_id, &new_page));
    CATCH(error, THROW(error))
    struct page_header *new_page_header = new_page.data;
    new_page_header->previous = old_last_page_id;
    new_page_header->next = PAGE_ID_NULL;
  }

  {
    // update `next` field in old last page
    page_t old_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, old_last_page_id, &old_page));
    CATCH(error, THROW(error))
    struct page_header *old_page_header = old_page.data;
    old_page_header->next = new_last_page_id;
  }
  OK;
}

result_t page_group_manager_create_group(struct page_group_manager *self,
                                         page_group_id_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  page_t new_page = PAGE_NULL;
  page_id_t new_page_id = PAGE_ID_NULL;
  TRY(create_page(self, &new_page, &new_page_id));
  CATCH(error, THROW(error))
  struct page_header *page_header = new_page.data;
  page_header->previous = new_page_id;

  *result = (page_group_id_t){new_page_id.bytes};
  OK;
}

static result_t page_group_manager_free_page(struct page_group_manager *self,
                                             page_id_t page_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct application_header *application_header =
      page_manager_get_application_header(self->page_manager);

  // get current last free page
  page_id_t old_last_page_id = application_header->last_free_page;

  // alter last free page pointer
  application_header->last_free_page = page_id;
  if (page_id_is_null(old_last_page_id)) {
    // if there were no free page beforehand - set page to be first one
    application_header->first_free_page = page_id;

    page_t page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, page_id, &page));
    CATCH(error, THROW(error))

    struct page_header *header = page.data;
    header->previous = PAGE_ID_NULL;
    header->next = PAGE_ID_NULL;
  } else {
    // otherwise set `next` & `previous` pointers in old & new last pages
    page_t old_last_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, old_last_page_id,
                              &old_last_page));
    CATCH(error, THROW(error))

    struct page_header *old_last_header = old_last_page.data;
    old_last_header->next = page_id;

    page_t page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, page_id, &page));
    CATCH(error, THROW(error))

    struct page_header *header = page.data;
    header->previous = old_last_page_id;
  }
  OK;
}

result_t page_group_manager_free_page_current(struct page_group_manager *self,
                                              struct page_iterator *it) {
  page_id_t page_id = it->current_page_id;

  page_t page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, page_id, &page));
  CATCH(error, THROW(error))

  // get previous & next page ids
  page_id_t prev_page_id = ((struct page_header *)page.data)->previous;
  page_id_t next_page_id = ((struct page_header *)page.data)->next;

  if (!page_id_is_null(prev_page_id)) {
    // if previous page exists update its next page
    page_t prev_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, prev_page_id, &prev_page));
    CATCH(error, THROW(error))

    ((struct page_header *)prev_page.data)->next = next_page_id;
  }
  if (!page_id_is_null(next_page_id)) {
    // if next page exists update its previous page
    page_t next_page = PAGE_NULL;
    TRY(page_manager_get_page(self->page_manager, next_page_id, &next_page));
    CATCH(error, THROW(error))

    ((struct page_header *)next_page.data)->previous = prev_page_id;
  }

  return page_group_manager_free_page(self, it->current_page_id);
}

result_t page_group_manager_delete_group(struct page_group_manager *self,
                                         page_group_id_t page_group_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  page_t page = PAGE_NULL;
  struct page_header *header = NULL;
  page_id_t current_page_id = (page_id_t){page_group_id.bytes};
  // just free each page in the group one by one
  while (!page_id_is_null(current_page_id)) {
    TRY(page_manager_get_page(self->page_manager, current_page_id, &page));
    CATCH(error, THROW(error))

    header = page.data;
    page_id_t next_page_id = header->next;
    TRY(page_group_manager_free_page(self, current_page_id));
    CATCH(error, THROW(error))

    current_page_id = next_page_id;
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
                             page_group_id_t page_group_id, bool inverse) {
  return page_iterator_new(self->page_manager, page_group_id, inverse);
}

result_t page_group_manager_get_page(struct page_group_manager *self,
                                     page_id_t page_id, page_t *result) {
  page_t page = PAGE_NULL;
  TRY(page_manager_get_page(self->page_manager, page_id, &page));
  CATCH(error, THROW(error))
  result->data = page.data + sizeof(struct page_header);
  OK;
}