cmake_minimum_required(VERSION 3.0)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

set(COMMON_LIB llp_lab_common_lib)
set(COMMON_HEADERS ${CMAKE_SOURCE_DIR}/common/include)

add_subdirectory(common)
add_subdirectory(database)
#add_subdirectory(parser)
enable_testing()
