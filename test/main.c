#include <malloc.h>
#include <stdio.h>

#include "private/error/error.h"
#include "public/storage/database_manager.h"

static void handle_error(struct error *err) {
  printf("Encountered error at %s:\n %s(%zu): %s", err->error_source,
         err->error_type, err->error_code.bytes, err->error_message);
  error_destroy(err);
}

int main() {
  result_t res;

  struct page_data_manager *page_data_manager = page_data_manager_new();

  res = page_data_manager_ctor(page_data_manager, "test_file");
  if (result_is_err(res)) {
    handle_error(res.error);
    return 0;
  }

  page_group_id_t page_group_id = PAGE_GROUP_ID_NULL;
  res = page_data_manager_create_group(page_data_manager, &page_group_id);
  if (result_is_err(res)) {
    page_data_manager_destroy(page_data_manager);
    handle_error(res.error);
    return 0;
  }

  printf("group id: %zu\n", page_group_id.bytes);

  uint16_t *item_data = malloc(sizeof(uint16_t));
  *item_data = 0xCD;
  item_t item = (item_t){.size = sizeof(uint16_t), .data = item_data};
  res = page_data_manager_insert(page_data_manager, page_group_id, item);
  free(item_data);
  if (result_is_err(res)) {
    page_data_manager_destroy(page_data_manager);
    handle_error(res.error);
    return 0;
  }

  res = page_data_manager_flush(page_data_manager);
  if (result_is_err(res)) {
    page_data_manager_destroy(page_data_manager);
    handle_error(res.error);
    return 0;
  }

  //  struct item_iterator *item_it =
  //      page_data_manager_get_items(page_data_manager, page_group_id);
  //
  //  while (item_iterator_has_next(item_it)) {
  //    printf("item");
  //    item_t item = ITEM_NULL;
  //    res = item_iterator_next(item_it, &item);
  //    if (result_is_err(res)) {
  //      item_iterator_destroy(item_it);
  //      page_data_manager_destroy(page_data_manager);
  //      handle_error(res.error);
  //      return 0;
  //    }
  //  }
  //  printf("no more items");
  //
  //  item_iterator_destroy(item_it);
  page_data_manager_destroy(page_data_manager);
  printf("done");
  return 0;
}
