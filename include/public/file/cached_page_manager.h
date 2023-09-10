//
// Created by draen on 08.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_FILE_CACHED_PAGE_MANAGER_H
#define LLP_LAB_INCLUDE_PUBLIC_FILE_CACHED_PAGE_MANAGER_H

#include "public/file/page_manager.h"
#include "public/file/page_resolver.h"
#include "public/util/result.h"

struct cached_page_manager;

struct cached_page_manager *cached_page_manager_new();

result_t
cached_page_manager_ctor(struct cached_page_manager *cached_page_manager,
                         struct i_page_resolver *page_resolver,
                         size_t cache_size);

#endif // LLP_LAB_INCLUDE_PUBLIC_FILE_CACHED_PAGE_MANAGER_H
