//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_VIEW_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_VIEW_H

#include "common/public/util/result.h"
#include "record_group.h"
#include <stdbool.h>

struct record_view;

struct table_schema *record_view_get_schema(struct record_view *self);
bool record_view_has_next(struct record_view *self);
result_t record_view_next(struct record_view *self, struct record **result);

void record_view_destroy(struct record_view *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_VIEW_H
