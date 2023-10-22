//
// Created by draen on 02.10.23.
//

#ifndef LLP_LAB_TEST_INCLUDE_DATABASE_MANAGER_WRAPPER_H
#define LLP_LAB_TEST_INCLUDE_DATABASE_MANAGER_WRAPPER_H

#include "database/prelude.h"

#define TEST_ERROR(Message)                                                    \
  error_new("test", "test", (error_code_t){0}, Message)

#define WRAP_WITH_DB(FileName, Function)                                       \
  struct database_manager *database_manager = database_manager_new();          \
  TRY(database_manager_ctor(database_manager, FileName));                      \
  CATCH(error, {                                                               \
    handle_error(error);                                                       \
    return 1;                                                                  \
  })                                                                           \
  TRY(Function(database_manager));                                             \
  CATCH(error, {                                                               \
    database_manager_destroy(database_manager);                                \
    handle_error(error);                                                       \
    return 1;                                                                  \
  })                                                                           \
  database_manager_destroy(database_manager);                                  \
  return 0

#endif // LLP_LAB_TEST_INCLUDE_DATABASE_MANAGER_WRAPPER_H
