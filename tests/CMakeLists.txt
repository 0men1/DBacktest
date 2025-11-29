file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(DBacktest_TEST ${TEST_SOURCES})

target_link_libraries(DBacktest_TEST PRIVATE 
    GTest::gtest_main 
    src 
    strategies 
)

include(GoogleTest)
gtest_discover_tests(DBacktest_TEST)
