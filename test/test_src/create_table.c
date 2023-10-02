//
// Created by draen on 02.10.23.
//
#include "common_table_operations.h"
#include "common_test_utils.h"
#include "public/prelude.h"

int main() { WRAP_WITH_DB("database", create_common_table); }