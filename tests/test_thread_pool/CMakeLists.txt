enable_testing()
include(GoogleTest)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

add_executable(TestThreadPool test_thread_pool.cpp)

target_link_libraries(TestThreadPool
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    ThreadPool
)

gtest_discover_tests(TestThreadPool)