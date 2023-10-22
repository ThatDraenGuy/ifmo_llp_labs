//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H

#include "common/public/util/result.h"
#include "database/public/database/domain/record/record_group.h"
#include "database/public/storage/domain/item.h"
#include "domain/schema.h"

void record_serialize_into(struct record *record, item_t target);
item_t record_serialize(struct record *record);
result_t record_deserialize(item_t item, struct record *target,
                            size_t start_column_index, size_t end_column_index);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H
