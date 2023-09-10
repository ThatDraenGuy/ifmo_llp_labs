#include <stdio.h>

#include "public/file/cached_page_manager.h"
#include "public/file/file_page_resolver.h"

int main() {
  struct file_page_resolver *file_page_resolver = file_page_resolver_new();
  if (file_page_resolver_ctor(file_page_resolver, "test_file") == RESULT_ERR) {
    printf("resolver err");
    return 0;
  }

  struct cached_page_manager *cached_page_manager = cached_page_manager_new();
  if (cached_page_manager_ctor(cached_page_manager,
                               (struct i_page_resolver *)file_page_resolver,
                               10) == RESULT_ERR) {
    printf("manager err");
    return 0;
  }

  page_manager_destroy((struct i_page_manager *)cached_page_manager);
  return 0;
}
