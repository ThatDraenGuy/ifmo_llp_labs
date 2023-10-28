project(llp_lab_parser C)

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.0 REQUIRED)

#project target
set(PARSER_LIB ${PROJECT_NAME}_lib)
set(PARSER_LIB ${PARSER_LIB} PARENT_SCOPE)

#project headers dir
set(PARSER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include)
set(PARSER_HEADERS ${PARSER_HEADERS} PARENT_SCOPE)

set(PARSER_DIR ${CMAKE_CURRENT_BINARY_DIR}/parser_gen)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)


set(LEXER_OUT "${PARSER_DIR}/lexer.c")
set(PARSER_OUT "${PARSER_DIR}/parser.c")

flex_target(LEXER "${SRC_DIR}/lexer.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lexer.tab.h")
bison_target(PARSER "${SRC_DIR}/parser.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.tab.h")
add_flex_bison_dependency(LEXER PARSER)


#project source files
file(GLOB_RECURSE SOURCES ${SRC_DIR}/**.c)

add_library(${PARSER_LIB} ${SOURCES} ${LEXER_OUT} ${PARSER_OUT})
target_link_libraries(${PARSER_LIB} PRIVATE ${COMMON_LIB})
target_include_directories(${PARSER_LIB} PRIVATE ${PARSER_HEADERS} ${PARSER_DIR} ${COMMON_HEADERS})

enable_testing()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)