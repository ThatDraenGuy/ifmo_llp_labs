//
// Created by draen on 15.09.23.
//

#include "private/database/table_manager.h"
#include "public/error/errors_common.h"
#include <malloc.h>

const char *const error_source = "TABLE_MANAGER";

struct table_manager *table_manager_new() {
  return malloc(sizeof(struct table_manager));
}

result_t table_manager_ctor(struct table_manager *self, char *file_name) {
  ASSERT_NOT_NULL(self, error_source);

  struct page_data_manager *page_data_manager = page_data_manager_new();

  TRY(page_data_manager_ctor(page_data_manager, file_name));
  CATCH(error, {
    free(self);
    PROPAGATE;
  })

  self->page_data_manager = page_data_manager;

  return OK;
}
