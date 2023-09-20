//
// Created by draen on 18.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_TABLE_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_TABLE_H

#include "public/database/domain/table.h"

struct table {
  page_group_id_t page_group_id;
  struct table_schema *table_schema;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_TABLE_H
