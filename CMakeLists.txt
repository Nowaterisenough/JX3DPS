cmake_minimum_required(VERSION 3.12)

include(${CMAKE_SOURCE_DIR}/cmake/version.cmake)
version()

project(JX3DPS VERSION ${PROJECT_VERSION})

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
print_bordered_info(${PROJECT_NAME} ${PROJECT_VERSION} ${VERSION_BRANCH})

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

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
option(BUILD_BENCHMARK "option for build" OFF)

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_BENCHMARK)
    add_subdirectory(benchmarks)
endif()