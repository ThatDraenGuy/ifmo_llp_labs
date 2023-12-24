project(llp_lab_database C)

#project target
set(PROJECT_LIB ${PROJECT_NAME}_lib)
set(DATABASE_LIB ${PROJECT_LIB} PARENT_SCOPE)

#project headers dir
set(HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(DATABASE_HEADERS ${HEADERS_DIR} PARENT_SCOPE)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)

#main project lib
add_library(${PROJECT_LIB} ${SOURCES})
target_link_libraries(${PROJECT_LIB} PRIVATE ${COMMON_LIB})
target_include_directories(${PROJECT_LIB} PRIVATE ${HEADERS_DIR} ${COMMON_HEADERS})


#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
ENDIF ()

enable_testing()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)