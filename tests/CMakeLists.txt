set(TEST_SOURCES
    metrics.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE ciri GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)