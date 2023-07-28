project(TestJX3DPS)

enable_testing()
include(GoogleTest)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)



add_definitions(-DIMPORT_STATIC_JX3DPS_LIB)

add_executable(${PROJECT_NAME} TestJX3DPS.cpp)
target_link_libraries(${PROJECT_NAME}
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    JX3DPS
    nlohmann_json
)

add_custom_command(
    TARGET ${PROJECT_NAME} # 指定编译的目标名
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/tests/${PROJECT_NAME}/TestTaiXuJianYi.json # 要复制的文件路径
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/TestTaiXuJianYi.json # 目标路径
)

gtest_discover_tests(TestJX3DPS)