set(TEST_SOURCES
    metrics.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE
    ciri
    GTest::gtest_main
)

if(ENABLE_ASAN)
    target_compile_definitions(tests PRIVATE
        ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
    )
endif()

include(GoogleTest)
gtest_discover_tests(tests)