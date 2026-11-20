cmake_minimum_required(VERSION 3.26)

project(JX3DPS LANGUAGES C CXX)

add_library(jx3dps_runtime_core INTERFACE)
target_include_directories(jx3dps_runtime_core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

option(JX3DPS_SHARED_ENGINE "Build the native engine as a shared C ABI library" OFF)
if(JX3DPS_SHARED_ENGINE AND NOT EMSCRIPTEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
add_subdirectory(src)

if(EMSCRIPTEN)
    add_subdirectory(apps/web)
    return()
endif()

option(JX3DPS_RUNTIME_ONLY "Build standalone runtime tests and microbenchmarks" OFF)
option(JX3DPS_DESKTOP_TESTS "Build Qt desktop integration tests" OFF)
if(JX3DPS_DESKTOP_TESTS)
    enable_testing()
endif()
if(JX3DPS_RUNTIME_ONLY)
    include(CTest)
    add_subdirectory(apps/cli)
    add_subdirectory(tests/runtime)
    add_subdirectory(tests/engine)
    return()
endif()

include(cmake/Version.cmake)
VERSION()

# 设置项目名称&版本
project(JX3DPS VERSION ${PROJECT_VERSION})
message(STATUS "-------------- PROJECT_VERSION: ${PROJECT_VERSION} --------------")
message(STATUS "-------------- VERSION_BRANCH: ${VERSION_BRANCH} --------------")

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

option(BUILD_CLI "Build standalone simulation command-line clients" ON)
if(BUILD_CLI)
    add_subdirectory(apps/cli)
endif()

# CMake选项 app编译
option(BUILD_APP "option for build" ON)

# CMake选项 UT测试
option(BUILD_TESTS "option for build" OFF)

option(BUILD_PERFORMANCE "Build performance benchmarks" OFF)
add_subdirectory(3rd)

# Resources 需要 Qt，仅在 BUILD_APP 时构建
if(BUILD_APP)
    add_subdirectory(resources)
endif()

set(BUILD_SHARED_LIBS true)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

if(BUILD_APP)
    # Check if Qt is available for app
    find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets PrintSupport OpenGLWidgets)
    if(QT_FOUND)
        message(STATUS "Qt found, building apps")
        add_subdirectory(apps)
    else()
        message(STATUS "Qt not found, skipping apps")
    endif()
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


if(BUILD_PERFORMANCE)
    add_subdirectory(performance)
endif()

# ============================================================================
# Configuration Summary
# ============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "JX3DPS Configuration Summary")
message(STATUS "========================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Branch:            ${VERSION_BRANCH}")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler:      ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "Core Components:")
message(STATUS "  JX3DPS Library:  ENABLED")
message(STATUS "  Simulation C ABI: ENABLED")
message(STATUS "  JSON (nlohmann): ENABLED")
message(STATUS "")
message(STATUS "Optional Components:")
if(BUILD_APP)
    if(QT_FOUND)
        message(STATUS "  Qt Framework:    ENABLED (${QT_VERSION})")
        message(STATUS "  Desktop App:     ENABLED")
        message(STATUS "    - Frameless:   ENABLED")
        message(STATUS "    - Theme:       ENABLED")
        message(STATUS "    - CodeEditor:  ENABLED")
        message(STATUS "    - Timeline:    ENABLED")
        message(STATUS "    - StatePanel:  ENABLED")
    else()
        message(STATUS "  Qt Framework:    NOT FOUND")
        message(STATUS "  Desktop App:     DISABLED")
    endif()
else()
    message(STATUS "  Desktop App:     DISABLED (BUILD_APP=OFF)")
endif()

if(BUILD_TESTS)
    message(STATUS "  Unit Tests:      ENABLED")
else()
    message(STATUS "  Unit Tests:      DISABLED (BUILD_TESTS=OFF)")
endif()

message(STATUS "")
message(STATUS "Install Prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Source Directory:  ${CMAKE_SOURCE_DIR}")
message(STATUS "Build Directory:   ${CMAKE_BINARY_DIR}")
message(STATUS "========================================")
message(STATUS "")
