project(llp_lab_parser_client C)

#project target
set(PARSER_CLIENT_APP ${PROJECT_NAME}_app)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/**.c)

add_executable(${PARSER_CLIENT_APP} ${SOURCES})
target_include_directories(${PARSER_CLIENT_APP} PRIVATE ${PARSER_HEADERS} ${COMMON_HEADERS})
target_link_libraries(${PARSER_CLIENT_APP} PRIVATE ${PARSER_LIB} ${COMMON_LIB})