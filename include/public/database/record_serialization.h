//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H

#include "domain/record_group.h"
#include "domain/schema.h"
#include "public/storage/domain/item.h"
#include "public/util/result.h"

item_t record_serialize(struct record *record);
result_t record_deserialize(item_t item, struct record *target);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_RECORD_SERIALIZATION_H
