//
// Created by draen on 28.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H

#include "record.h"
#include "schema.h"

struct single_record_holder;
struct single_record_holder *single_record_holder_new();
void single_record_holder_ctor(struct single_record_holder *self,
                               size_t table_schema_num, ...);
struct record *
single_record_holder_get_values(struct single_record_holder *self);
void single_record_holder_clear(struct single_record_holder *self);
void single_record_holder_destroy(struct single_record_holder *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_SINGLE_RECORD_HOLDER_H
