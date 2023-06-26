cmake_minimum_required(VERSION 3.15)

include(cmake/Version.cmake)
VERSION()

# 设置项目名称&版本
project(JX3DPS VERSION ${PROJECT_VERSION})
message(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")
message(STATUS "VERSION_BRANCH: ${VERSION_BRANCH}")

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# install路径
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        if(WIN32)
            set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
        else()
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory" FORCE)
        endif()
    else()
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# CMake选项 动态库编译
option(BUILD_SHARED_LIBS "option for build" ON)

if(BUILD_SHARED_LIBS)
    set(BUILD_LIBS_TYPE SHARED)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
    set(BUILD_LIBS_TYPE STATIC)
endif()

# if(POLICY CMP0091)
# cmake_policy(SET CMP0091 NEW)
# endif(POLICY CMP0091)

# CMake选项 MSVC MD编译
if(MSVC)
    option(BUILD_MSVC_MD "option for build" ON)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /permissive /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive /utf-8")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GR- /GS- /GL /Ot /Oi /Ob2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR- /GS- /GL /Ot /Oi /Ob2 /Zc:__cplusplus")

    if(BUILD_MSVC_MD)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif(BUILD_MSVC_MD)
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -fPIC -fPIE -fno-exceptions -fno-rtti")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -fPIC -fPIE -fno-exceptions -fno-rtti")
endif(MSVC)

add_subdirectory(3rd)
add_subdirectory(modules)

# CMake选项 app编译
option(BUILD_APP "option for build" ON)

if(BUILD_APP)
    add_subdirectory(apps)
endif()

# CMake选项 UT测试
option(BUILD_TESTS "option for build" OFF)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
