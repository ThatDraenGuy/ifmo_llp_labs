//
// Created by draen on 08.09.23.
//

#include "public/file/file_manager.h"
#include "private/file/file_manager.h"
#include "public/error/errors_common.h"
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#define ERROR_SOURCE "FILE_MANAGER"
#define ERROR_TYPE "FILE_MANAGER_ERROR"

static struct error *error_self() {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){errno},
                   strerror(errno));
}

struct file_manager *file_manager_new() {
  return malloc(sizeof(struct file_manager));
}

result_t file_manager_ctor(struct file_manager *self, char *file_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  bool file_exists = access(file_name, F_OK) == 0;
  if (!file_exists) {
    // TODO think
    fclose(fopen(file_name, "a"));
  }

  FILE *file = fopen(file_name, "r+");

  if (file == NULL) {
    free(self);
    THROW(error_self());
  }

  self->file = file;
  self->is_new = !file_exists;
  OK;
}

bool file_manager_is_file_new(struct file_manager *self) {
  return self->is_new;
}

result_t file_manager_read(struct file_manager *self, size_t size,
                           uint32_t offset, void *data) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  if (fseek(self->file, offset, SEEK_SET) != 0) {
    THROW(error_self());
  }

  if (fread(data, size, 1, self->file) == 0)
    THROW(error_self()); // TODO think
  OK;
}

result_t file_manager_write(struct file_manager *self, size_t size,
                            uint32_t offset, void *data) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  if (fseek(self->file, offset, SEEK_SET) != 0)
    THROW(error_self());

  if (fwrite(data, size, 1, self->file) == 0)
    THROW(error_self()); // TODO think
  OK;
}

void file_manager_destroy(struct file_manager *self) {
  fclose(self->file);
  free(self);
}