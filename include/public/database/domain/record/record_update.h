//
// Created by draen on 30.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_UPDATE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_UPDATE_H

#include "public/util/result.h"
#include "public/util/string.h"
#include <stddef.h>

struct record_update;
struct record_update *record_update_new();
void record_update_ctor(struct record_update *self, str_t table_name,
                        size_t columns_num, ...);
void record_update_destroy(struct record_update *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_UPDATE_H
