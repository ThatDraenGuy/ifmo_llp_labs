//
// Created by draen on 09.09.23.
//

#include "private/file/page_resolver.h"
#include "public/error/errors_common.h"

static const char *const error_source = "PAGE_RESOLVER";

size_t page_resolver_get_page_size(struct i_page_resolver *self) {
  if (self == NULL)
    return 0;
  return self->get_page_size_impl(self);
}

void *page_resolver_get_application_header(struct i_page_resolver *self) {
  if (self == NULL)
    return NULL;
  return self->get_application_header_impl(self);
}

result_t page_resolver_get_new_page_id(struct i_page_resolver *self,
                                       page_id_t *result) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));
  return self->get_new_page_id_impl(self, result);
}

result_t page_resolver_read_page(struct i_page_resolver *self,
                                 page_id_t page_id, page_t destination) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));
  return self->read_page_impl(self, page_id, destination);
}

result_t page_resolver_write_page(struct i_page_resolver *self,
                                  page_id_t page_id, page_t data) {
  if (self == NULL)
    return result_err(error_common(error_source, ERR_COMMON_NULL_POINTER));
  return self->write_page_impl(self, page_id, data);
}
void page_resolver_destroy(struct i_page_resolver *self) {
  if (self == NULL)
    return;
  self->destroy_impl(self);
}