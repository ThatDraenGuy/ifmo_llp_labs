//
// Created by draen on 07.09.23.
//

#include "public/file/page_manager.h"
#include "private/file/page_manager.h"
#include "public/error/errors_common.h"
#include "public/util/result.h"

static const char *const error_source = "PAGE_MANAGER";

void *page_manager_get_application_header(struct i_page_manager *self) {
  if (self == NULL)
    return NULL;
  return self->get_application_header_impl(self);
}

result_t page_manager_create_page(struct i_page_manager *self, page_t *result,
                                  page_id_t *result_id) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));
  return self->create_page_impl(self, result, result_id);
}

result_t page_manager_get_page(struct i_page_manager *self, page_id_t page_id,
                               page_t *result) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));
  return self->get_page_impl(self, page_id, result);
}

size_t page_manager_get_page_size(struct i_page_manager *self) {
  if (self == NULL)
    return 0;
  return self->get_page_size_impl(self);
}

result_t page_manager_flush(struct i_page_manager *self) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));

  return self->flush_impl(self);
}

void page_manager_destroy(struct i_page_manager *self) {
  if (self == NULL)
    return;
  self->destroy_impl(self);
}