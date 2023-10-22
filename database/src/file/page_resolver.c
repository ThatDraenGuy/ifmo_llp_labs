//
// Created by draen on 08.09.23.
//

#include "database/private/file/page_resolver.h"
#include "common/public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

#define ERROR_SOURCE "PAGE_RESOLVER"
#define ERROR_TYPE "PAGE_RESOLVER_ERROR"

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
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  *result_id = (page_id_t){++self->file_header.page_amount};
  self->was_file_header_altered = true;
  OK;
}

result_t page_resolver_read_page(struct page_resolver *self, page_id_t page_id,
                                 page_t destination) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  size_t page_size = page_resolver_get_page_size(self);
  uint32_t offset = resolve_page_offset(self, page_size, page_id);

  return file_manager_read(self->file_manager, page_size, offset,
                           destination.data);
}

result_t page_resolver_write_page(struct page_resolver *self, page_id_t page_id,
                                  page_t data) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  if (self->was_file_header_altered) {
    TRY(write_header(self));
    CATCH(error, THROW(error))
  }
  size_t page_size = page_resolver_get_page_size(self);
  uint32_t offset = resolve_page_offset(self, page_size, page_id);

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
  ASSERT_NOT_NULL(self, ERROR_SOURCE);

  struct file_manager *file_manager = file_manager_new();
  TRY(file_manager_ctor(file_manager, file_name));
  CATCH(error, {
    free(self);
    THROW(error);
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
      THROW(error);
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
      THROW(error);
    })

    if (file_header->format_type != FORMAT_TYPE) {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      THROW(error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){INVALID_HEADER},
                      error_messages[INVALID_HEADER]));
    }

    TRY(file_manager_read(file_manager, application_header_size,
                          sizeof(struct file_header), application_header));
    CATCH(error, {
      free(application_header);
      file_manager_destroy(file_manager);
      free(self);
      THROW(error);
    })
  }

  OK;
}