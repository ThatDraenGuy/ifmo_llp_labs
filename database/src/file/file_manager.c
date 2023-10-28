//
// Created by draen on 08.09.23.
//

#include "database/public/file/file_manager.h"
#include "common/public/error/errors_common.h"
#include "database/private/file/file_manager.h"
#include <errno.h>
#include <malloc.h>
#include <string.h>

#ifdef WINDOWS
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

#define ERROR_SOURCE STR_OF("FILE_MANAGER")
#define ERROR_TYPE STR_OF("FILE_MANAGER_ERROR")

static struct error *error_self() {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){errno},
                   str_of(strerror(errno)));
}

struct file_manager *file_manager_new() {
  return malloc(sizeof(struct file_manager));
}

result_t file_manager_ctor(struct file_manager *self, char *file_name) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  bool file_exists = access(file_name, F_OK) == 0;
  if (!file_exists) {
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
      free(self);
      THROW(error_self());
    }
    if (fclose(file) != 0) {
      free(self);
      THROW(error_self());
    }
  }

  FILE *file = fopen(file_name, "rb+");

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
    THROW(error_self());
  OK;
}

result_t file_manager_write(struct file_manager *self, size_t size,
                            uint32_t offset, void *data) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  if (fseek(self->file, offset, SEEK_SET) != 0)
    THROW(error_self());

  if (fwrite(data, size, 1, self->file) == 0)
    THROW(error_self());
  OK;
}

void file_manager_destroy(struct file_manager *self) {
  fclose(self->file);
  free(self);
}