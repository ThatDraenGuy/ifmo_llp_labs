//
// Created by draen on 15.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H

#include "public/database/table_manager.h"
#include "public/storage/page_data_manager.h"

STR(TABLE_DATA_TABLE_NAME, "TABLE_DATA_TABLE")
STR(TABLE_DATA_TABLE_COLUMN_TABLE_ID, "TABLE_ID")
STR(TABLE_DATA_TABLE_COLUMN_TABLE_NAME, "TABLE_NAME")
STR(TABLE_DATA_TABLE_COLUMN_PAGE_GROUP_ID, "PAGE_GROUP_ID")
STR(TABLE_DATA_TABLE_COLUMN_COLUMNS_AMOUNT, "COLUMN_AMOUNT")

STR(TABLE_COLUMNS_TABLE_NAME, "TABLE_COLUMNS_TABLE")
STR(TABLE_COLUMNS_TABLE_COLUMN_TABLE_ID, "TABLE_ID")
STR(TABLE_COLUMNS_TABLE_COLUMN_NAME, "NAME")
STR(TABLE_COLUMNS_TABLE_COLUMN_TYPE, "TYPE")

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
//
// struct record_iterator {
//  struct item_iterator *main_item_it;
//  struct table *table;
//  struct where *where;
//  struct record *current_record;
//  struct record *next_record;
//  bool is_empty;
//};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_TABLE_MANAGER_H
