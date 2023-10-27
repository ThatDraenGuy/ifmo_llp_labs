//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H

#include "database/public/database/table_manager.h"
#include "database/public/storage/page_data_manager.h"
#define TABLE_DATA_TABLE_NAME STR_OF("TABLE_DATA_TABLE")

#define TABLE_DATA_TABLE_COLUMN_TABLE_ID STR_OF("TABLE_ID")
#define TABLE_DATA_TABLE_COLUMN_TABLE_NAME STR_OF("TABLE_NAME")
#define TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID STR_OF("PAGE_GROUP_ID")
#define TABLE_DATA_TABLE_COLUMN_COLUMNS_AMOUNT STR_OF("COLUMN_AMOUNT")

#define TABLE_COLUMNS_TABLE_NAME STR_OF("TABLE_COLUMNS_TABLE")
#define TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID STR_OF("TABLE_ID")
#define TABLE_COLUMNS_TABLE_COLUMN_NAME STR_OF("NAME")
#define TABLE_COLUMNS_TABLE_COLUMN_TYPE STR_OF("TYPE")

struct meta_contents {
  page_group_id_t table_data_table_page_group_id;
  page_group_id_t table_columns_table_page_group_id;
  uint64_t next_table_id;
};

struct table_manager {
  struct page_data_manager *page_data_manager;
  struct table *table_data_table;
  struct table *table_columns_table;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
