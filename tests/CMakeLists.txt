file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(DBacktest_TEST ${TEST_SOURCES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(DBacktest_TEST PRIVATE 
    TEST_RESOURCE_DIR="${CMAKE_CURRENT_BINARY_DIR}/test_data/"
)

target_link_libraries(DBacktest_TEST PRIVATE 
    GTest::gtest_main
    src
)

include(GoogleTest)
gtest_discover_tests(DBacktest_TEST)
