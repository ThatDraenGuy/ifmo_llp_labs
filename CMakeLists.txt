cmake_minimum_required(VERSION 3.0)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(common)
add_subdirectory(database)
add_subdirectory(parser)
add_subdirectory(client)
enable_testing()
