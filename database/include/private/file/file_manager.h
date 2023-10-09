//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_MANAGER_H

#include "public/file/file_manager.h"
#include <stdbool.h>
#include <stdio.h>

struct file_manager {
  FILE *file;
  bool is_new;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_FILE_FILE_MANAGER_H
