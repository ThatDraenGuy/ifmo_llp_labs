project(llp_lab_common C)

#project target
#set(PROJECT_LIB ${PROJECT_NAME}_lib)

#project headers dir
#set(HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

#project source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)

#main project lib
add_library(${COMMON_LIB} ${SOURCES})
target_include_directories(${COMMON_LIB} PRIVATE ${COMMON_HEADERS})


#windows compatability setup
IF (${WINDOWS})
    add_compile_definitions(WINDOWS)
ENDIF ()
