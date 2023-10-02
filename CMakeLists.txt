cmake_minimum_required(VERSION 3.0)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

#project target
set(PROJECT_LIB ${PROJECT_NAME}_lib)

#project headers dir
set(HEADERS_DIR ${CMAKE_SOURCE_DIR}/include)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)

#main project lib
add_library(${PROJECT_LIB} ${SOURCES})
target_include_directories(${PROJECT_LIB} PRIVATE ${HEADERS_DIR})


#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
ENDIF ()

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/test)