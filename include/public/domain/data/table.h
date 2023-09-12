//
// Created by draen on 10.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DOMAIN_DATA_TABLE_H
#define LLP_LAB_INCLUDE_PUBLIC_DOMAIN_DATA_TABLE_H

#include "public/domain/schema/table_schema.h"
#include "public/storage/page_group.h"

struct table {
  struct table_schema *schema;
  page_group_id_t page_group_id;
};

#endif // LLP_LAB_INCLUDE_PUBLIC_DOMAIN_DATA_TABLE_H
