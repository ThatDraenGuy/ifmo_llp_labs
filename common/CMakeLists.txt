project(llp_lab_common C)

#project target
set(COMMON_LIB ${PROJECT_NAME}_lib)
set(COMMON_LIB ${COMMON_LIB} PARENT_SCOPE)

#project headers dir
set(COMMON_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include)
set(COMMON_HEADERS ${COMMON_HEADERS} PARENT_SCOPE)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)

#main project lib
add_library(${COMMON_LIB} ${SOURCES})
target_include_directories(${COMMON_LIB} PRIVATE ${COMMON_HEADERS})


#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
ENDIF ()
