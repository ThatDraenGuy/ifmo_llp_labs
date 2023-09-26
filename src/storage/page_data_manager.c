//
// Created by draen on 08.09.23.
//

#include "private/storage/page_data_manager.h"
#include "public/error/errors_common.h"
#include "public/storage/page_data_manager.h"
#include <malloc.h>
#include <memory.h>

#define ERROR_SOURCE "PAGE_DATA_MANAGER"

bool item_iterator_has_next(struct item_iterator *self) {
  if (self->is_empty)
    return false;
  struct page_data_header *page_header = self->current_page.data;
  if (self->next_item_index < page_header->item_amount)
    return true;

  return page_iterator_has_next(self->page_iterator);
}

result_t item_iterator_next(struct item_iterator *self, item_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct page_data_header *page = self->current_page.data;

  if (!item_iterator_has_next(self))
    THROW(error_common(ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  if (self->next_item_index >= page->item_amount) {
    TRY(page_iterator_next(self->page_iterator, &self->current_page));
    CATCH(error, THROW(error))

    self->next_item_index = 0;
    page = self->current_page.data;
  }

  page_item_id_t page_item_id =
      (page_item_id_t)(page->contents + self->next_item_index *
                                            sizeof(struct page_item_id_data));
  self->next_item_index++;

  self->current_item =
      (item_t){.size = page_item_id->item_size.bytes,
               page->contents + page_item_id->item_offset.bytes};

  *result = self->current_item;
  OK;
}

void item_iterator_destroy(struct item_iterator *self) {
  page_iterator_destroy(self->page_iterator);
  free(self);
}

static struct item_iterator *
item_iterator_new(struct page_data_manager *page_data_manager,
                  page_group_id_t page_group_id) {
  struct item_iterator *it = malloc(sizeof(struct item_iterator));
  it->page_iterator = page_group_manager_get_group(
      page_data_manager->page_group_manager, page_group_id);
  it->current_item = ITEM_NULL;
  it->next_item_index = 0;

  if (page_iterator_has_next(it->page_iterator)) {
    TRY(page_iterator_next(it->page_iterator, &it->current_page));
    CATCH(error, {
      it->is_empty = true;
      return it;
    })

    struct page_data_header *header = it->current_page.data;
    if (header->item_amount == 0) {
      it->is_empty = true;
      return it;
    }
  } else {
    it->is_empty = true;
    return it;
  }

  it->is_empty = false;
  return it;
}

static result_t insert_item(struct page_data_header *page, item_t item) {
  size_t item_offset = page->free_space_end.bytes - item.size;

  page_item_id_t item_id =
      (page_item_id_t)(page->contents + page->free_space_start.bytes);
  void *new_item_data = page->contents + item_offset;

  page->free_space_start.bytes += sizeof(struct page_item_id_data);
  page->free_space_end.bytes = item_offset;
  page->item_amount++;

  item_id->item_size.bytes = item.size;
  item_id->item_offset.bytes = item_offset;
  item_id->is_deleted = false;

  memcpy(new_item_data, item.data, item.size);
  OK;
}

static void initialize_page(struct page_data_manager *self,
                            struct page_data_header *page_header) {
  page_header->free_space_start = (page_index_t){.bytes = 0};
  page_header->free_space_end = (page_index_t){
      .bytes = page_group_manager_get_page_capacity(self->page_group_manager) -
               sizeof(struct page_data_header)};
  page_header->item_amount = 0;
}

static result_t vacuum_page(struct page_data_header *page) {
  page_index_t gap_id_index = (page_index_t){0};
  page_index_t gap_data_index = (page_index_t){0};
  bool vacuum_needed = false;
  uint16_t actual_item_amount = page->item_amount;

  for (uint16_t index = 0; index < page->item_amount; index++) {
    page_index_t item_id_offset =
        (page_index_t){index * sizeof(struct page_item_id_data)};
    page_item_id_t item_id =
        (page_item_id_t)(page->contents + item_id_offset.bytes);

    if (item_id->is_deleted)
      actual_item_amount--;

    if (item_id->is_deleted && !vacuum_needed) {
      // vacuum start condition
      vacuum_needed = true;
      gap_id_index = item_id_offset;
      gap_data_index.bytes =
          item_id->item_offset.bytes + item_id->item_size.bytes;

    } else if (!item_id->is_deleted && vacuum_needed) {
      // perform vacuum

      page_index_t old_item_offset = item_id->item_offset;
      page_index_t new_item_offset =
          (page_index_t){gap_data_index.bytes - item_id->item_size.bytes};
      // move actual item to fill the gap
      memmove(page->contents + new_item_offset.bytes,
              page->contents + old_item_offset.bytes, item_id->item_size.bytes);

      // adjust item offset
      item_id->item_offset = new_item_offset;

      // move item id to fill the gap
      memcpy(page->contents + gap_id_index.bytes,
             page->contents + item_id_offset.bytes,
             sizeof(struct page_item_id_data));

      // adjust new gap parameters
      gap_id_index = item_id_offset;
      gap_data_index.bytes -= item_id->item_size.bytes;
    }
  }

  page->item_amount = actual_item_amount;

  OK;
}

static result_t vacuum_deleted_items(struct page_data_manager *self,
                                     page_group_id_t page_group_id) {
  struct page_iterator *page_it =
      page_group_manager_get_group(self->page_group_manager, page_group_id);

  while (page_iterator_has_next(page_it)) {
    page_t page = PAGE_NULL;
    TRY(page_iterator_next(page_it, &page));
    CATCH(error, {
      page_iterator_destroy(page_it);
      THROW(error);
    })

    struct page_data_header *page_data_header = page.data;
    TRY(vacuum_page(page_data_header));
    CATCH(error, {
      page_iterator_destroy(page_it);
      THROW(error);
    })
  }

  page_iterator_destroy(page_it);
  OK;
}

struct page_data_manager *page_data_manager_new() {
  return malloc(sizeof(struct page_data_manager));
}

result_t page_data_manager_ctor(struct page_data_manager *self,
                                char *file_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  self->page_group_manager = page_group_manager_new();

  TRY(page_group_manager_ctor(self->page_group_manager, file_name));
  CATCH(error, {
    free(self);
    THROW(error);
  })

  OK;
}

page_group_id_t
page_data_manager_get_meta_group_id(struct page_data_manager *self) {
  return page_group_manager_get_meta_group_id(self->page_group_manager);
}

result_t page_data_manager_set_meta_group_id(struct page_data_manager *self,
                                             page_group_id_t page_group_id) {
  return page_group_manager_set_meta_group_id(self->page_group_manager,
                                              page_group_id);
}

result_t page_data_manager_create_group(struct page_data_manager *self,
                                        page_group_id_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  TRY(page_group_manager_create_group(self->page_group_manager, result));
  CATCH(error, THROW(error))

  struct page_iterator *page_it =
      page_group_manager_get_group(self->page_group_manager, *result);

  if (page_iterator_has_next(page_it)) {
    page_t page = PAGE_NULL;
    TRY(page_iterator_next(page_it, &page));
    CATCH(error, {
      page_iterator_destroy(page_it);
      THROW(error);
    })

    struct page_data_header *page_header = page.data;
    initialize_page(self, page_header);
  }
  page_iterator_destroy(page_it);
  OK;
}

result_t page_data_manager_delete_group(struct page_data_manager *self,
                                        page_group_id_t page_group_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  return page_group_manager_delete_group(self->page_group_manager,
                                         page_group_id);
}

result_t page_data_manager_insert(struct page_data_manager *self,
                                  page_group_id_t page_group_id, item_t item) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct page_iterator *page_it =
      page_group_manager_get_group(self->page_group_manager, page_group_id);

  while (page_iterator_has_next(page_it)) {
    page_t page = PAGE_NULL;
    TRY(page_iterator_next(page_it, &page));
    CATCH(error, THROW(error))

    struct page_data_header *page_header = page.data;

    if (page_item_entry_size(item.size) <
        page_header->free_space_end.bytes -
            page_header->free_space_start.bytes) {
      page_iterator_destroy(page_it);
      return insert_item(page_header, item);
    }
  }

  page_t page = PAGE_NULL;
  TRY(page_group_manager_add_page(self->page_group_manager, page_it, &page));
  CATCH(error, THROW(error))

  struct page_data_header *page_header = page.data;

  page_iterator_destroy(page_it);

  initialize_page(self, page_header);
  return insert_item(page_header, item);
}

result_t page_data_manager_flush(struct page_data_manager *self,
                                 page_group_id_t page_group_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  TRY(vacuum_deleted_items(self, page_group_id));
  CATCH(error, THROW(error))
  return page_group_manager_flush(self->page_group_manager);
}

void page_data_manager_destroy(struct page_data_manager *self) {
  page_group_manager_destroy(self->page_group_manager);
  free(self);
}

void item_iterator_delete_item(struct item_iterator *self) {
  struct page_data_header *page = self->current_page.data;

  page_item_id_t page_item_id =
      (page_item_id_t)(page->contents + self->next_item_index *
                                            sizeof(struct page_item_id_data));
  page_item_id->is_deleted = true;
}

struct item_iterator *
page_data_manager_get_items(struct page_data_manager *self,
                            page_group_id_t page_group_id) {
  return item_iterator_new(self, page_group_id);
}