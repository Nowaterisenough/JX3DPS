cmake_minimum_required(VERSION 3.12)

project(JX3DPS LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_TESTING OFF CACHE BOOL "Build tests")

add_subdirectory(3rd)

file(GLOB SOURCES "src/*.c*" "src/*.h*")

add_definitions(-DEXPORT_JX3DPS)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC
    nlohmann_json
    spdlog
    msft_proxy
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 

option(BUILD_EXAMPLE "option for build" ON)
option(BUILD_BENCHMARK "option for build" ON)

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_BENCHMARK)
    add_subdirectory(benchmarks)
endif()