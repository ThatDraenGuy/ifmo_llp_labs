cmake_minimum_required(VERSION 3.26)
project(llp_lab C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/**.h)
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/test/**.c)

include_directories(include)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} PUBLIC ${HEADERS})

add_library(${PROJECT_NAME}_lib ${SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE clang_rt.asan-x86_64)
ENDIF ()