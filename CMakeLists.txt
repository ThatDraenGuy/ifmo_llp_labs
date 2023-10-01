cmake_minimum_required(VERSION 3.26)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

#project targets
set(PROJECT_LIB ${PROJECT_NAME}_lib)
SET(PROJECT_TEST ${PROJECT_NAME}_test)

#project headers dir
set(HEADERS_DIR include)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/test/**.c)

#main project code
add_library(${PROJECT_LIB} ${SOURCES})
target_include_directories(${PROJECT_LIB} PRIVATE ${HEADERS_DIR})

#test executable
add_executable(${PROJECT_TEST} ${TEST_SOURCES})
target_link_libraries(${PROJECT_TEST} PRIVATE ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_TEST} PRIVATE ${HEADERS_DIR})

#test setup
enable_testing()
add_test(
        NAME test
        COMMAND $<TARGET_FILE:${PROJECT_TEST}>
)

#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
    target_link_libraries(${PROJECT_TEST} PRIVATE clang_rt.asan_dynamic-x86_64 clang_rt.asan_dynamic_runtime_thunk-x86_64)
ELSE ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
ENDIF ()