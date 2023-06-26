enable_testing()
include(GoogleTest)

add_executable(test TestThreadPool.cpp)
target_link_libraries(test
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    ThreadPool
)

gtest_discover_tests(test)