//
// Created by draen on 08.09.23.
//

#include "private/file/file_page_resolver.h"
#include <malloc.h>

static size_t get_page_size(void *this) {
  struct file_page_resolver *file_page_resolver = this;
  return file_page_resolver->file_header.page_size;
}

static result_t read_page(void *this, page_id_t page_id, page_t destination) {
  struct file_page_resolver *file_page_resolver = this;
  size_t page_size =
      page_resolver_get_page_size((struct i_page_resolver *)file_page_resolver);
  uint32_t offset = file_page_resolver->file_header.offset_to_data +
                    page_size * page_id.bytes;

  return file_manager_read(file_page_resolver->file_manager, page_size, offset,
                           destination.data);
}

static result_t write_page(void *this, page_id_t page_id, page_t data) {
  struct file_page_resolver *file_page_resolver = this;
  // TODO
}

static void destroy(void *this) {
  struct file_page_resolver *file_page_resolver = this;
  file_manager_destroy(file_page_resolver->file_manager);
  free(file_page_resolver);
}

static result_t initialize_file_header(struct file_manager *file_manager,
                                       struct file_header *file_header) {
  file_header->format_type = FORMAT_TYPE;
  file_header->offset_to_data = sizeof(struct file_header);
  file_header->page_size = DEFAULT_PAGE_SIZE;
  file_header->page_amount = 0;
  return file_manager_write(file_manager, sizeof(struct file_header), 0,
                            file_header);
}

struct file_page_resolver *file_page_resolver_new() {
  return malloc(sizeof(struct file_page_resolver));
}

result_t file_page_resolver_ctor(struct file_page_resolver *file_page_resolver,
                                 char *file_name) {
  struct file_manager *file_manager = file_manager_new();
  if (file_manager_ctor(file_manager, file_name) == RESULT_ERR) {
    free(file_page_resolver);
    return RESULT_ERR;
  }

  struct file_header *file_header = &(file_page_resolver->file_header);

  if (file_manager_is_file_new(file_manager)) {
    if (initialize_file_header(file_manager, file_header) == RESULT_ERR) {
      free(file_page_resolver);
      file_manager_destroy(file_manager);
      return RESULT_ERR;
    }
  } else {
    if (file_manager_read(file_manager, sizeof(struct file_header), 0,
                          file_header) == RESULT_ERR) {
      free(file_page_resolver);
      file_manager_destroy(file_manager);
      return RESULT_ERR;
    }

    if (file_header->format_type != FORMAT_TYPE) {
      free(file_page_resolver);
      file_manager_destroy(file_manager);
      return RESULT_ERR;
    }
  }

  file_page_resolver->file_manager = file_manager;

  file_page_resolver->parent.get_page_size_impl = get_page_size;
  file_page_resolver->parent.read_page_impl = read_page;
  file_page_resolver->parent.write_page_impl = write_page;
  file_page_resolver->parent.destroy_impl = destroy;

  return RESULT_OK;
}