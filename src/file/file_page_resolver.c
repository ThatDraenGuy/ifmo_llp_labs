//
// Created by draen on 08.09.23.
//

#include "private/file/file_page_resolver.h"
#include <malloc.h>
#include <memory.h>

static const char *const error_source = "FILE_PAGE_RESOLVER";
static const char *const error_type = "FILE_PAGE_RESOLVER_ERROR";

enum error_code { INVALID_HEADER = 0 };

static const char *const error_messages[] = {[INVALID_HEADER] =
                                                 "File header is invalid!"};

static void *get_application_header(void *self) {
  struct file_page_resolver *file_page_resolver = self;
  return file_page_resolver->application_header;
}

static size_t get_page_size(void *self) {
  struct file_page_resolver *file_page_resolver = self;
  return file_page_resolver->file_header.page_size;
}

static result_t get_new_page_id(void *self, page_id_t *result) {
  struct file_page_resolver *file_page_resolver = self;
  *result = (page_id_t){file_page_resolver->file_header.page_amount + 1};
  return result_ok();
}

static uint32_t resolve_page_offset(struct file_page_resolver *self,
                                    size_t page_size, page_id_t page_id) {
  return self->file_header.offset_to_data + page_size * page_id.bytes;
}

static result_t read_page(void *self, page_id_t page_id, page_t destination) {
  struct file_page_resolver *file_page_resolver = self;
  size_t page_size =
      page_resolver_get_page_size((struct i_page_resolver *)self);
  uint32_t offset = resolve_page_offset(file_page_resolver, page_size, page_id);

  return file_manager_read(file_page_resolver->file_manager, page_size, offset,
                           destination.data);
}

static result_t write_page(void *self, page_id_t page_id, page_t data) {
  struct file_page_resolver *file_page_resolver = self;
  size_t page_size =
      page_resolver_get_page_size((struct i_page_resolver *)self);
  uint32_t offset = resolve_page_offset(file_page_resolver, page_size, page_id);

  return file_manager_write(file_page_resolver->file_manager, page_size, offset,
                            data.data);
}

static void destroy(void *self) {
  struct file_page_resolver *file_page_resolver = self;
  file_manager_destroy(file_page_resolver->file_manager);
  free(file_page_resolver->application_header);
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

result_t file_page_resolver_ctor(struct file_page_resolver *self,
                                 char *file_name, size_t header_size,
                                 void *default_header) {
  result_t res;

  struct file_manager *file_manager = file_manager_new();
  res = file_manager_ctor(file_manager, file_name);
  if (result_is_err(res)) {
    free(self);
    return res;
  }

  self->file_manager = file_manager;
  self->application_header_size = header_size;
  self->application_header = malloc(header_size);

  struct file_header *file_header = &(self->file_header);
  void *application_header = self->application_header;

  if (file_manager_is_file_new(file_manager)) {
    // file is new - initialize headers
    res = initialize_file_header(file_manager, file_header);
    if (result_is_err(res)) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return res;
    }
    memcpy(application_header, default_header, header_size);

    res = file_manager_write(file_manager, header_size,
                             sizeof(struct file_header), application_header);
    if (result_is_err(res)) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return res;
    }
  } else {
    // file is not new - read headers
    res = file_manager_read(file_manager, sizeof(struct file_header), 0,
                            file_header);
    if (result_is_err(res)) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return res;
    }

    if (file_header->format_type != FORMAT_TYPE) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return result_err(error_new(error_source, error_type,
                                  (error_code_t){INVALID_HEADER},
                                  error_messages[INVALID_HEADER]));
    }

    res = file_manager_read(file_manager, header_size,
                            sizeof(struct file_header), application_header);
    if (result_is_err(res)) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return res;
    }
  }

  self->parent.get_application_header_impl = get_application_header;
  self->parent.get_page_size_impl = get_page_size;
  self->parent.get_new_page_id_impl = get_new_page_id;
  self->parent.read_page_impl = read_page;
  self->parent.write_page_impl = write_page;
  self->parent.destroy_impl = destroy;

  return result_ok();
}