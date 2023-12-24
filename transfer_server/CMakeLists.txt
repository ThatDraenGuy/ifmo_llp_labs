project(llp_lab_transfer_server C)

#project target
set(TRANSFER_SERVER_APP ${PROJECT_NAME}_app)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/**.c)

set(TRANSFER_SERVER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 thrift_c_glib)

add_executable(${TRANSFER_SERVER_APP} ${SOURCES})
target_include_directories(${TRANSFER_SERVER_APP} PRIVATE ${TRANSFER_SERVER_HEADERS}
        ${COMMON_HEADERS} ${TRANSFER_HEADERS} ${DATABASE_HEADERS})
target_link_libraries(${TRANSFER_SERVER_APP} PRIVATE ${DATABASE_LIB} ${COMMON_LIB} ${TRANSFER_LIB} PkgConfig::deps)
