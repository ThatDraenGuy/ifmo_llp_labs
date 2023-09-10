//
// Created by draen on 08.09.23.
//

#include "public/file/file_manager.h"
#include "private/file/file_manager.h"
#include <malloc.h>
#include <unistd.h>

struct file_manager *file_manager_new() {
  return malloc(sizeof(struct file_manager));
}

result_t file_manager_ctor(struct file_manager *file_manager, char *file_name) {
  bool file_exists = access(file_name, F_OK) == 0;
  if (!file_exists) {
    // TODO think
    fclose(fopen(file_name, "a"));
  }

  FILE *file = fopen(file_name, "r+");

  if (file == NULL) {
    free(file_manager);
    return RESULT_ERR;
  }

  file_manager->file = file;
  file_manager->is_new = !file_exists;
  return RESULT_OK;
}

bool file_manager_is_file_new(struct file_manager *file_manager) {
  return file_manager->is_new;
}

result_t file_manager_read(struct file_manager *file_manager, size_t size,
                           uint32_t offset, void *data) {
  if (file_manager == NULL)
    return RESULT_ERR;
  if (fseek(file_manager->file, offset, SEEK_SET) != 0)
    return RESULT_ERR;

  if (fread(data, size, 1, file_manager->file) == 0)
    return RESULT_ERR;
  return RESULT_OK;
}

result_t file_manager_write(struct file_manager *file_manager, size_t size,
                            uint32_t offset, void *data) {
  if (file_manager == NULL)
    return RESULT_ERR;
  if (fseek(file_manager->file, offset, SEEK_SET) != 0)
    return RESULT_ERR;

  if (fwrite(data, size, 1, file_manager->file) == 0)
    return RESULT_ERR;
  return RESULT_OK;
}

void file_manager_destroy(struct file_manager *file_manager) {
  fclose(file_manager->file);
  free(file_manager);
}