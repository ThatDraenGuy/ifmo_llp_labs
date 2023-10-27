//
// Created by draen on 02.10.23.
//

#ifndef LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H
#define LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H

#include "database/prelude.h"

#define TABLE_NAME STR_OF("MY_COOL_TABLE")
#define ID_COL STR_OF("ID")
#define STATUS_COL STR_OF("STATUS")
#define NUM_COL STR_OF("NUM")

struct table_schema *create_common_schema();

result_t create_common_table(struct database_manager *database_manager);
result_t populate_common_table(struct database_manager *database_manager,
                               uint64_t record_num);

#endif // LLP_LAB_TEST_INCLUDE_CREATE_COMMON_TABLE_H
