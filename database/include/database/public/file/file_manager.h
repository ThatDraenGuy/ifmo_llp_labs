//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_MANAGER_H

#include "common/public/util/result.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct file_manager;

struct file_manager *file_manager_new();

result_t file_manager_ctor(struct file_manager *self, char *file_name);

bool file_manager_is_file_new(struct file_manager *self);

result_t file_manager_read(struct file_manager *self, size_t size,
                           uint32_t offset, void *data);

result_t file_manager_write(struct file_manager *self, size_t size,
                            uint32_t offset, void *data);

void file_manager_destroy(struct file_manager *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_FILE_MANAGER_H
