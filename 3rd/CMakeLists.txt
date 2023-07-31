

if(BUILD_TESTS)
    add_subdirectory(googletest)
endif()

add_subdirectory(json)
add_subdirectory(spdlog)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spdlog PRIVATE -w)
endif()