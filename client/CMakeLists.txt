project(llp_lab_client C)

#project target
set(CLIENT_APP ${PROJECT_NAME}_app)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/**.c)

add_executable(${CLIENT_APP} ${SOURCES})
target_include_directories(${CLIENT_APP} PRIVATE ${PARSER_HEADERS} ${COMMON_HEADERS})
target_link_libraries(${CLIENT_APP} PRIVATE ${PARSER_LIB} ${COMMON_LIB})