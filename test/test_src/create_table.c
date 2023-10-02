//
// Created by draen on 02.10.23.
//
#include "create_common_table.h"
#include "database_manager_wrapper.h"
#include "public/prelude.h"

int main() { WRAP_WITH_DB("database", create_common_table); }