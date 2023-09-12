#include <stdio.h>

#include "public/storage/database_manager.h"

int main() {
  result_t res;

  struct database_manager *database_manager = database_manager_new();

  res = database_manager_ctor(database_manager, "test_file");
  if (result_is_err(res)) {
    printf("err");
    error_destroy(res.error);
    return 0;
  }

  return 0;
}
