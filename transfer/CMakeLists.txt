project(llp_lab_transfer C)

#project target
set(TRANSFER_LIB ${PROJECT_NAME}_lib)
set(TRANSFER_LIB ${TRANSFER_LIB} PARENT_SCOPE)

#project headers dir
set(TRANSFER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include)
set(TRANSFER_HEADERS ${TRANSFER_HEADERS} PARENT_SCOPE)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 thrift_c_glib)

#main project lib
add_library(${TRANSFER_LIB} ${SOURCES})
target_include_directories(${TRANSFER_LIB} PRIVATE ${TRANSFER_HEADERS})
target_link_libraries(${TRANSFER_LIB} PkgConfig::deps)


#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
ENDIF ()
