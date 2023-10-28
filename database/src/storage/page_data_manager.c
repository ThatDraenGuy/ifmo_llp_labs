//
// Created by draen on 08.09.23.
//

#include "database/private/storage/page_data_manager.h"
#include "common/public/error/errors_common.h"
#include "database/public/storage/page_data_manager.h"
#include <malloc.h>
#include <memory.h>
#include <string.h>

#define ERROR_SOURCE STR_OF("PAGE_DATA_MANAGER")
#define ERROR_TYPE STR_OF("PAGE_DATA_MANAGER_ERROR")
enum error_code { ITEM_SIZE_EXCEEDED };
static const str_t error_messages[] = {[ITEM_SIZE_EXCEEDED] =
                                           STR_OF("Item size exceeded!")};

bool item_iterator_has_next(struct item_iterator *self) {
  return !self->is_empty;
}

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static result_t item_iterator_preload_next_item(struct item_iterator *self) {
  page_item_id_t next_item_id;
  do {
    self->next_item_index++;
    // get current page
    page_t page = PAGE_NULL;
    TRY(page_iterator_current(self->page_iterator, &page));
    CATCH(error, THROW(error))
    struct page_data_header *page_data = page.data;

    if (page_data->item_amount <= self->next_item_index) {
      // if no more items in page - go to next page
      self->next_item_index = 0;
      if (!page_iterator_has_next(self->page_iterator)) {
        // if no more pages - iterator is empty, quit trying to find next
        self->is_empty = true;
        OK;
      }
      TRY(page_iterator_next(self->page_iterator, &page));
      CATCH(error, THROW(error))
      page_data = page.data;
    }
    // get next possible item info
    next_item_id = (page_item_id_t)(page_data->contents +
                                    self->next_item_index *
                                        sizeof(struct page_item_id_data));
    // if item is invalid (new or deleted) go to fetch next one
  } while (page_item_is_deleted(next_item_id) ||
           page_item_is_new(next_item_id));
  // item was found - return
  OK;
}

result_t item_iterator_next(struct item_iterator *self, item_t *result) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (!item_iterator_has_next(self))
    THROW(error_common(ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  // set preloaded next item info as current
  self->current_item_index = self->next_item_index;
  self->current_item_page = page_iterator_current_id(self->page_iterator);
  // preload next item info
  item_iterator_preload_next_item(self);

  // resolve current item and set it as result
  page_t page = PAGE_NULL;
  TRY(page_group_manager_get_page(self->page_group_manager,
                                  self->current_item_page, &page));
  CATCH(error, THROW(error))

  struct page_data_header *page_data = page.data;
  page_item_id_t item_id =
      (page_item_id_t)(page_data->contents +
                       self->current_item_index *
                           sizeof(struct page_item_id_data));

  *result = (item_t){.size = item_id->item_size.bytes,
                     .data = page_data->contents + item_id->item_offset.bytes};
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
  it->page_group_manager = page_data_manager->page_group_manager;
  it->page_iterator = page_group_manager_get_group(
      page_data_manager->page_group_manager, page_group_id, false);
  it->next_item_index = -1;
  it->current_item_page = PAGE_ID_NULL;
  it->current_item_index = 0;

  if (page_iterator_has_next(it->page_iterator)) {
    it->is_empty = false;
    page_t page = PAGE_NULL;
    page_iterator_next(it->page_iterator, &page);
    // if there are pages - preload next item
    item_iterator_preload_next_item(it);
  } else {
    // otherwise iterator is empty
    it->is_empty = true;
  }
  return it;
}

static result_t insert_item(struct page_data_header *page, item_t item,
                            bool set_new) {
  size_t item_offset = page->free_space_end.bytes - item.size;

  page_item_id_t item_id =
      (page_item_id_t)(page->contents + page->free_space_start.bytes);
  void *new_item_data = page->contents + item_offset;

  page->free_space_start.bytes += sizeof(struct page_item_id_data);
  page->free_space_end.bytes = item_offset;
  page->item_amount++;

  item_id->item_size.bytes = item.size;
  item_id->item_offset.bytes = item_offset;
  item_id->flags = 0;
  if (set_new)
    page_item_set_new(item_id);

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

#define AT_PAGE(Page, Index) (Page->contents + Index.bytes)
#define NEXT_PAGE_DATA(PageData, PageIt)                                       \
  {                                                                            \
    page_t page = PAGE_NULL;                                                   \
    TRY(page_iterator_next(PageIt, &page));                                    \
    CATCH(error, THROW(error))                                                 \
    PageData = page.data;                                                      \
  }
#define CURR(PageData, PageIt)                                                 \
  {                                                                            \
    page_t page = PAGE_NULL;                                                   \
    TRY(page_iterator_current(PageIt, &page));                                 \
    CATCH(error, THROW(error))                                                 \
    PageData = page.data;                                                      \
  }

static result_t vacuum(struct page_group_manager *page_group_manager,
                       struct page_iterator *src_page_it,
                       struct page_iterator *dest_page_it) {
  page_index_t dest_item_index = {0};
  page_index_t dest_item_id_index = {0};
  page_index_t src_item_index = {0};
  page_index_t src_item_id_index = {0};

  struct page_data_header *src_page_data;
  struct page_data_header *dest_page_data;
  bool vacuum_started = false;
  while (page_iterator_has_next(src_page_it)) {
    NEXT_PAGE_DATA(src_page_data, src_page_it)
    if (!vacuum_started)
      NEXT_PAGE_DATA(dest_page_data, dest_page_it)

    page_index_t iter_num = src_page_data->free_space_start;
    for (src_item_id_index.bytes = 0; src_item_id_index.bytes < iter_num.bytes;
         src_item_id_index.bytes += sizeof(struct page_item_id_data)) {
      // iterate over all items in src_page
      page_item_id_t src_item_id =
          (page_item_id_t)(src_page_data->contents + src_item_id_index.bytes);
      src_item_index = src_item_id->item_offset;

      if (page_item_is_new(src_item_id))
        // unset new flag
        page_item_unset_new(src_item_id);

      if (page_item_is_deleted(src_item_id))
        // adjust item amounts
        src_page_data->item_amount--;

      if (!vacuum_started && page_item_is_deleted(src_item_id)) {
        // start vacuum process
        dest_item_id_index = src_item_id_index;
        dest_item_index.bytes =
            src_item_id->item_offset.bytes + src_item_id->item_size.bytes;
        vacuum_started = true;
      } else if (vacuum_started && !page_item_is_deleted(src_item_id)) {
        if (page_item_entry_size(src_item_id->item_size.bytes) >
            dest_item_index.bytes - dest_item_id_index.bytes) {
          // no more space in this page
          dest_page_data->free_space_start = dest_item_id_index;
          dest_page_data->free_space_end = dest_item_index;
          // move dest pointers to the next page
          dest_item_id_index.bytes = 0;
          NEXT_PAGE_DATA(dest_page_data, dest_page_it)

          if (src_item_id_index.bytes == 0 &&
              page_iterator_current_id(src_page_it).bytes ==
                  page_iterator_current_id(dest_page_it).bytes) {
            // case where vacuum was started at previous page
            // and item from next page didn't actually fit
            // stop vacuum procedure because we don't have any free space
            vacuum_started = false;
            continue;
          }

          // if vacuum continues - set item destination to end of the page
          dest_item_index =
              (page_index_t){.bytes = page_group_manager_get_page_capacity(
                                          page_group_manager) -
                                      sizeof(struct page_data_header)};
        }
        CURR(dest_page_data, dest_page_it)
        // move item data
        memmove(AT_PAGE(dest_page_data,
                        (page_index_t){dest_item_index.bytes -
                                       src_item_id->item_size.bytes}),
                AT_PAGE(src_page_data, src_item_index),
                src_item_id->item_size.bytes);

        // update item id
        src_item_id->item_offset = (page_index_t){dest_item_index.bytes -
                                                  src_item_id->item_size.bytes};

        // move item id
        memcpy(AT_PAGE(dest_page_data, dest_item_id_index),
               AT_PAGE(src_page_data, src_item_id_index),
               sizeof(struct page_item_id_data));

        if (page_iterator_current_id(src_page_it).bytes !=
            page_iterator_current_id(dest_page_it).bytes) {
          dest_page_data->item_amount++;
          src_page_data->item_amount--;
        }

        dest_item_index.bytes -= src_item_id->item_size.bytes;
        dest_item_id_index.bytes += sizeof(struct page_item_id_data);
      }
    }
  }
  // src_page_it ended
  dest_page_data->free_space_start = dest_item_id_index;
  dest_page_data->free_space_end = dest_item_index;
  while (page_iterator_has_next(dest_page_it)) {
    NEXT_PAGE_DATA(dest_page_data, dest_page_it)
    TRY(page_group_manager_free_page_current(page_group_manager, dest_page_it));
    CATCH(error, THROW(error))
  }

  OK;
}

static result_t vacuum_page(struct page_group_manager *page_group_manager,
                            struct page_iterator *page_it,
                            page_group_id_t page_group_id) {
  bool vacuum_needed = false;

  struct page_data_header *page_data;
  {
    page_t page = PAGE_NULL;
    TRY(page_iterator_current(page_it, &page));
    CATCH(error, THROW(error))
    page_data = page.data;
  }

  uint16_t initial_item_amount = page_data->item_amount;
  for (uint16_t index = 0; index < initial_item_amount; index++) {
    // get item info
    page_index_t item_id_offset =
        (page_index_t){index * sizeof(struct page_item_id_data)};
    page_item_id_t item_id =
        (page_item_id_t)(page_data->contents + item_id_offset.bytes);

    // reset new items
    if (page_item_is_new(item_id))
      page_item_unset_new(item_id);

    // count deleted items
    if (page_item_is_deleted(item_id))
      page_data->item_amount--;

    if (page_item_is_deleted(item_id) && !vacuum_needed) {
      // vacuum start condition
      vacuum_needed = true;
      // alter free space start & end pointers
      page_data->free_space_start = item_id_offset;
      page_data->free_space_end.bytes =
          item_id->item_offset.bytes + item_id->item_size.bytes;

    } else if (!page_item_is_deleted(item_id) && vacuum_needed) {
      // perform vacuum

      page_index_t old_item_offset = item_id->item_offset;
      page_index_t new_item_offset = (page_index_t){
          page_data->free_space_end.bytes - item_id->item_size.bytes};
      // move actual item to fill the gap
      memmove(page_data->contents + new_item_offset.bytes,
              page_data->contents + old_item_offset.bytes,
              item_id->item_size.bytes);

      // adjust item offset
      item_id->item_offset = new_item_offset;

      // move item id to fill the gap
      memcpy(page_data->contents + page_data->free_space_start.bytes,
             page_data->contents + item_id_offset.bytes,
             sizeof(struct page_item_id_data));

      // adjust new gap parameters
      page_data->free_space_start.bytes += sizeof(struct page_item_id_data);
      page_data->free_space_end.bytes -= item_id->item_size.bytes;
    }
  }

  if (page_data->item_amount == 0 &&
      page_iterator_current_id(page_it).bytes != page_group_id.bytes) {
    // if page became empty - free it and not bother with moving items from
    // other pages here
    TRY(page_group_manager_free_page_current(page_group_manager, page_it));
    CATCH(error, THROW(error))
  }

  OK;
}

result_t page_data_manager_vacuum(struct page_data_manager *self,
                                  page_group_id_t page_group_id) {
  struct page_iterator *src_page_it = page_group_manager_get_group(
      self->page_group_manager, page_group_id, false);
  struct page_iterator *dest_page_it = page_group_manager_get_group(
      self->page_group_manager, page_group_id, false);

  TRY(vacuum(self->page_group_manager, src_page_it, dest_page_it));
  CATCH(error, {
    page_iterator_destroy(src_page_it);
    page_iterator_destroy(dest_page_it);
    THROW(error);
  })
  page_iterator_destroy(src_page_it);
  page_iterator_destroy(dest_page_it);
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
      page_group_manager_get_group(self->page_group_manager, *result, false);

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

size_t max_item_size(struct page_data_manager *self) {
  return page_group_manager_get_page_capacity(self->page_group_manager) -
         sizeof(struct page_item_id_data);
}

result_t page_data_manager_insert(struct page_data_manager *self,
                                  page_group_id_t page_group_id, item_t item,
                                  bool immediate) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (item.size > max_item_size(self))
    THROW(error_self(ITEM_SIZE_EXCEEDED));

  struct page_iterator *page_it = page_group_manager_get_group(
      self->page_group_manager, page_group_id, true);

  size_t counter = 0;
  while (page_iterator_has_next(page_it)) {
    counter++;
    if (counter == 3)
      break;
    page_t page = PAGE_NULL;
    TRY(page_iterator_next(page_it, &page));
    CATCH(error, {
      page_iterator_destroy(page_it);
      THROW(error);
    })

    struct page_data_header *page_header = page.data;

    if (page_item_entry_size(item.size) <
        page_header->free_space_end.bytes -
            page_header->free_space_start.bytes) {
      page_iterator_destroy(page_it);
      return insert_item(page_header, item, !immediate);
    }
  }

  page_t page = PAGE_NULL;
  TRY(page_group_manager_add_page(self->page_group_manager, page_group_id,
                                  &page));
  CATCH(error, {
    page_iterator_destroy(page_it);
    THROW(error);
  })

  struct page_data_header *page_header = page.data;

  page_iterator_destroy(page_it);

  initialize_page(self, page_header);
  return insert_item(page_header, item, !immediate);
}

result_t page_data_manager_flush(struct page_data_manager *self,
                                 page_group_id_t page_group_id) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  TRY(page_data_manager_vacuum(self, page_group_id));
  CATCH(error, THROW(error))
  return page_group_manager_flush(self->page_group_manager);
}

void page_data_manager_destroy(struct page_data_manager *self) {
  page_group_manager_destroy(self->page_group_manager);
  free(self);
}

result_t item_iterator_delete_item(struct item_iterator *self) {
  page_t page = PAGE_NULL;
  TRY(page_group_manager_get_page(self->page_group_manager,
                                  self->current_item_page, &page));
  CATCH(error, THROW(error))
  struct page_data_header *page_header = page.data;

  page_item_id_t page_item_id =
      (page_item_id_t)(page_header->contents +
                       self->current_item_index *
                           sizeof(struct page_item_id_data));
  page_item_set_deleted(page_item_id);
  OK;
}

struct item_iterator *
page_data_manager_get_items(struct page_data_manager *self,
                            page_group_id_t page_group_id) {
  return item_iterator_new(self, page_group_id);
}