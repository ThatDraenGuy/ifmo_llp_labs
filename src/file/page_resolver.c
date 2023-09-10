//
// Created by draen on 09.09.23.
//

#include "private/file/page_resolver.h"

size_t page_resolver_get_page_size(struct i_page_resolver *page_resolver) {
  if (page_resolver == NULL)
    return 0;
  return page_resolver->get_page_size_impl(page_resolver);
}
result_t page_resolver_read_page(struct i_page_resolver *page_resolver,
                                 page_id_t page_id, page_t destination) {
  if (page_resolver == NULL)
    return RESULT_ERR;
  return page_resolver->read_page_impl(page_resolver, page_id, destination);
}

result_t page_resolver_write_page(struct i_page_resolver *page_resolver,
                                  page_id_t page_id, page_t data) {
  if (page_resolver == NULL)
    return RESULT_ERR;
  return page_resolver->write_page_impl(page_resolver, page_id, data);
}
void page_resolver_destroy(struct i_page_resolver *page_resolver) {
  if (page_resolver == NULL)
    return;
  page_resolver->destroy_impl(page_resolver);
}