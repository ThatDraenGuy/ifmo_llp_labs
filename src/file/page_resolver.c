//
// Created by draen on 08.09.23.
//

#include "private/file/page_resolver.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

static const char *const error_source = "PAGE_RESOLVER";
static const char *const error_type = "PAGE_RESOLVER_ERROR";

enum error_code { INVALID_HEADER = 0 };

static const char *const error_messages[] = {[INVALID_HEADER] =
                                                 "File header is invalid!"};

static uint32_t resolve_page_offset(struct page_resolver *self,
                                    size_t page_size, page_id_t page_id) {
  return self->file_header.offset_to_data + page_size * (page_id.bytes - 1);
}

static result_t write_header(struct page_resolver *self) {
  self->was_file_header_altered = false;
  return file_manager_write(self->file_manager, sizeof(struct file_header), 0,
                            &self->file_header);
}

size_t page_resolver_get_page_size(struct page_resolver *self) {
  return self->file_header.page_size;
}

void *page_resolver_get_application_header(struct page_resolver *self) {
  return self->application_header;
}

result_t page_resolver_flush_application_header(struct page_resolver *self) {
  return file_manager_write(self->file_manager, self->application_header_size,
                            sizeof(struct file_header),
                            self->application_header);
}

result_t page_resolver_get_new_page_id(struct page_resolver *self,
                                       page_id_t *result_id) {
  ASSERT_NOT_NULL(self, error_source);
  *result_id = (page_id_t){++self->file_header.page_amount};
  self->was_file_header_altered = true;
  return OK;
}

result_t page_resolver_read_page(struct page_resolver *self, page_id_t page_id,
                                 page_t destination) {
  ASSERT_NOT_NULL(self, error_source);
  size_t page_size = page_resolver_get_page_size(self);
  uint32_t offset = resolve_page_offset(self, page_size, page_id);

  return file_manager_read(self->file_manager, page_size, offset,
                           destination.data);
}

result_t page_resolver_write_page(struct page_resolver *self, page_id_t page_id,
                                  page_t data) {
  ASSERT_NOT_NULL(self, error_source);

  if (self->was_file_header_altered) {
    TRY(write_header(self));
    CATCH(error, PROPAGATE)
  }
  size_t page_size = page_resolver_get_page_size(self);
  uint32_t offset = resolve_page_offset(self, page_size, page_id);

  // TODO remove debug
  {
    struct file_manager *test_file_manager = file_manager_new();
    char str[16];
    sprintf(str, "page%zu", page_id.bytes);
    file_manager_ctor(test_file_manager, str);
    file_manager_write(test_file_manager, page_size, 0, data.data);
    file_manager_destroy(test_file_manager);
  }

  return file_manager_write(self->file_manager, page_size, offset, data.data);
}

void page_resolver_destroy(struct page_resolver *self) {
  file_manager_destroy(self->file_manager);
  free(self->application_header);
  free(self);
}

static result_t initialize_file_header(struct file_manager *file_manager,
                                       struct file_header *file_header,
                                       size_t application_header_size) {
  file_header->format_type = FORMAT_TYPE;
  file_header->offset_to_data =
      sizeof(struct file_header) + application_header_size;
  file_header->page_size = DEFAULT_PAGE_SIZE;
  file_header->page_amount = 0;
  return file_manager_write(file_manager, sizeof(struct file_header), 0,
                            file_header);
}

struct page_resolver *page_resolver_new() {
  return malloc(sizeof(struct page_resolver));
}

result_t page_resolver_ctor(struct page_resolver *self, char *file_name,
                            size_t application_header_size,
                            void *default_header) {
  ASSERT_NOT_NULL(self, error_source);

  struct file_manager *file_manager = file_manager_new();
  TRY(file_manager_ctor(file_manager, file_name));
  CATCH(error, {
    free(self);
    PROPAGATE;
  })

  self->file_manager = file_manager;
  self->application_header_size = application_header_size;
  self->application_header = malloc(application_header_size);

  struct file_header *file_header = &(self->file_header);
  void *application_header = self->application_header;

  if (file_manager_is_file_new(file_manager)) {
    // file is new - initialize headers
    self->was_file_header_altered = true;
    TRY(initialize_file_header(file_manager, file_header,
                               application_header_size));
    CATCH(error, {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      PROPAGATE;
    })
    memcpy(application_header, default_header, application_header_size);

    TRY(file_manager_write(file_manager, application_header_size,
                           sizeof(struct file_header), application_header));
    CATCH(error, {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
    })
  } else {
    // file is not new - read headers
    self->was_file_header_altered = false;
    TRY(file_manager_read(file_manager, sizeof(struct file_header), 0,
                          file_header));
    CATCH(error, {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      PROPAGATE;
    })

    if (file_header->format_type != FORMAT_TYPE) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      return result_err(error_new(error_source, error_type,
                                  (error_code_t){INVALID_HEADER},
                                  error_messages[INVALID_HEADER]));
    }

    TRY(file_manager_read(file_manager, application_header_size,
                          sizeof(struct file_header), application_header));
    CATCH(error, {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      PROPAGATE;
    })
  }

  return OK;
}