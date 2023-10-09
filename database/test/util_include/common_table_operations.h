//
// Created by draen on 02.10.23.
//

#ifndef LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H
#define LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H

#include "public/prelude.h"

STR(TABLE_NAME, "MY_COOL_TABLE")
STR(ID_COL, "ID")
STR(STATUS_COL, "STATUS")
STR(NUM_COL, "NUM")

struct table_schema *create_common_schema();

result_t create_common_table(struct database_manager *database_manager);
result_t populate_common_table(struct database_manager *database_manager,
                               uint64_t record_num);

#endif // LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H
