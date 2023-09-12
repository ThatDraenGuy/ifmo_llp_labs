//
// Created by draen on 08.09.23.
//

#include "private/storage/page_data_manager.h"
#include "public/error/errors_common.h"
#include "public/storage/page_data_manager.h"
#include <malloc.h>
#include <memory.h>

const char *const error_source = "PAGE_DATA_MANAGER";

bool item_iterator_has_next(struct item_iterator *self) {
  struct page_header *page_header = self->current_page.data;
  if (page_header->free_space_start.bytes >
      self->current_item_id_index.bytes + sizeof(page_item_id_data_t))
    return true;

  return page_iterator_has_next(self->page_iterator);
}

result_t item_iterator_next(struct item_iterator *self, item_t *result) {
  result_t res;

  struct page_header *page = self->current_page.data;

  if (!item_iterator_has_next(self))
    return result_err(error_common(error_source, ERR_COMMON_ITER_OUT_OF_RANGE));

  if (page->free_space_start.bytes >
      self->current_item_id_index.bytes + sizeof(page_item_id_data_t)) {
    self->current_item_id_index.bytes += sizeof(page_item_id_data_t);
  } else {
    res = page_iterator_next(self->page_iterator, &self->current_page);
    if (result_is_err(res))
      return res;

    self->current_item_id_index.bytes = 0;
  }
  page_item_id_t page_item_id =
      (page_item_id_t)(page->contents + self->current_item_id_index.bytes);
  self->current_item = (item_t){.size = page_item_id->item_size,
                                page->contents + page_item_id->item_offset};

  *result = self->current_item;
  return result_ok();
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

  return it;
}

static result_t insert_item(struct page_header *page, item_t item) {
  size_t item_offset = page->free_space_end.bytes - item.size;

  page_item_id_t item_id =
      (page_item_id_t)(page->contents + page->free_space_start.bytes);
  void *new_item_data = page->contents + item_offset;

  page->free_space_start.bytes += sizeof(page_item_id_data_t);
  page->free_space_end.bytes = item_offset;

  item_id->item_size = item.size;
  item_id->item_offset = item_offset;

  memcpy(new_item_data, item.data, item.size);
  return result_ok();
}

result_t page_data_manager_insert(struct page_data_manager *self,
                                  page_group_id_t page_group_id, item_t item) {
  result_t res;

  struct page_iterator *page_it =
      page_group_manager_get_group(self->page_group_manager, page_group_id);

  while (page_iterator_has_next(page_it)) {
    page_t page = PAGE_NULL;
    res = page_iterator_next(page_it, &page);
    if (result_is_err(res))
      return res;

    struct page_header *page_header = page.data;

    if (page_item_entry_size(item.size) <
        page_header->free_space_end.bytes -
            page_header->free_space_start.bytes) {
      page_iterator_destroy(page_it);
      return insert_item(page_header, item);
    }
  }

  page_t page = PAGE_NULL;
  res = page_group_manager_add_page(self->page_group_manager, page_it, &page);
  if (result_is_err(res))
    return res;

  struct page_header *page_header = page.data;

  page_iterator_destroy(page_it);

  page_header->free_space_start = (page_index_t){.bytes = 0}; // TODO think
  page_header->free_space_end = (page_index_t){
      .bytes = page_group_manager_get_page_size(self->page_group_manager)};
  return insert_item(page_header, item);
}

void item_iterator_delete_item(struct item_iterator *self) {
  struct page_header *page = self->current_page.data;

  page_item_id_t page_item_id =
      (page_item_id_t)(page->contents + self->current_item_id_index.bytes);
  page_item_id->is_deleted = true;
}

struct item_iterator *
page_data_manager_get_items(struct page_data_manager *self,
                            page_group_id_t page_group_id) {
  return item_iterator_new(self, page_group_id);
}