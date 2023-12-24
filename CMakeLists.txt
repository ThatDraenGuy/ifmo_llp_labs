cmake_minimum_required(VERSION 3.27)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(common)
add_subdirectory(database)
add_subdirectory(parser)
add_subdirectory(parser_client)
add_subdirectory(transfer)
add_subdirectory(transfer_client)
add_subdirectory(transfer_server)

enable_testing()
