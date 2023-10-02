#setup working dir for all tests
set(TESTS_WORKING_DIR ${CMAKE_BINARY_DIR}/tests)
file(MAKE_DIRECTORY ${TESTS_WORKING_DIR})

#test project id
set(PROJECT_TEST ${PROJECT_NAME}_test)

set(PROJECT_TEST_UTIL_LIB ${PROJECT_TEST}_lib)

#test sources
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/test_src/**.c)
file(GLOB_RECURSE TEST_UTIL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/util_src/**.c)

#test util headers
set(TEST_UTIL_HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/util_include)

#test util lib
add_library(${PROJECT_TEST_UTIL_LIB} ${TEST_UTIL_SOURCES})
target_link_libraries(${PROJECT_TEST_UTIL_LIB} PRIVATE ${PROJECT_LIB})
target_include_directories(${PROJECT_TEST_UTIL_LIB} PRIVATE ${HEADERS_DIR})
target_include_directories(${PROJECT_TEST_UTIL_LIB} PRIVATE ${TEST_UTIL_HEADERS_DIR})


#create a test for each test source
foreach (TEST_SOURCE ${TEST_SOURCES})
    #get test name
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)


    #setup test executable id & working dir
    set(TEST_ID ${PROJECT_TEST}_${TEST_NAME})
    set(TEST_WORKING_DIR ${TESTS_WORKING_DIR}/${TEST_NAME})
    file(MAKE_DIRECTORY ${TEST_WORKING_DIR})

    #create test executable
    add_executable(${TEST_ID} ${TEST_SOURCE})
    target_link_libraries(${TEST_ID} PRIVATE ${PROJECT_LIB} ${PROJECT_TEST_UTIL_LIB})
    target_include_directories(${TEST_ID} PRIVATE ${HEADERS_DIR})
    target_include_directories(${TEST_ID} PRIVATE ${TEST_UTIL_HEADERS_DIR})

    #setup test
    add_test(
            NAME ${TEST_NAME}
            COMMAND ${CMAKE_COMMAND} -DTEST_EXECUTABLE=$<TARGET_FILE:${TEST_ID}> -DTEST_WORKING_DIR=${TEST_WORKING_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/runtest.cmake
            WORKING_DIRECTORY ${TEST_WORKING_DIR}
    )
endforeach ()