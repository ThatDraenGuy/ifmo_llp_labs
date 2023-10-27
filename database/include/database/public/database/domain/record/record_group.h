//
// Created by draen on 26.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_GROUP_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_GROUP_H

#include "common/public/domain/column_types.h"
#include "common/public/util/result.h"
#include "database/public/database/domain/schema.h"
#include "record.h"
#include <stdbool.h>

struct record_iterator;
struct record_group;

struct record_group *record_group_new();
result_t record_group_ctor(struct record_group *self, size_t table_schema_num,
                           ...);

result_t record_group_insert(struct record_group *self, size_t values_num, ...);

void record_group_clear(struct record_group *self);

struct record_iterator *record_group_get_records(struct record_group *self);

result_t record_group_get_record(struct record_group *self, size_t index,
                                 struct record **result);

bool record_iterator_has_next(struct record_iterator *self);

result_t record_iterator_next(struct record_iterator *self,
                              struct record **result);

void record_iterator_destroy(struct record_iterator *self);

void record_group_destroy(struct record_group *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_GROUP_H
