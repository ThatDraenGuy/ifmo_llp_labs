//
// Created by draen on 07.09.23.
//

#include "public/file/page_manager.h"
#include "private/file/page_manager.h"
#include "public/util/result.h"
#include <malloc.h>

result_t page_manager_get_page(struct i_page_manager *page_manager,
                               page_id_t page_id, page_t *result) {
  if (page_manager == NULL)
    return RESULT_ERR;
  return page_manager->get_page_impl(page_manager, page_id, result);
}

size_t page_manager_get_page_size(struct i_page_manager *page_manager) {
  if (page_manager == NULL)
    return 0;
  return page_manager->get_page_size_impl(page_manager);
}

void page_manager_destroy(struct i_page_manager *page_manager) {
  if (page_manager == NULL)
    return;
  page_manager->destroy_impl(page_manager);
}