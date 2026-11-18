enable_testing()
include(GoogleTest)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

add_executable(test_thread_pool test_thread_pool.cpp)

target_link_libraries(test_thread_pool
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    thread_pool
)

gtest_discover_tests(test_thread_pool)