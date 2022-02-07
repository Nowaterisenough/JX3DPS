cmake_minimum_required(VERSION 3.5)

include(cmake/utils.cmake)                                                      
VERSION()

# 设置项目名称&版本
project(JX3DPS VERSION ${PROJECT_VERSION})
message(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")
message(STATUS "VERSION_BRANCH: ${VERSION_BRANCH}")

#set(CMAKE_C_FLAGS_RELEASE "-Wall -O2 -fPIC -fPIE -fno-exceptions -fno-rtti")
#set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -fPIC -fPIE -fno-exceptions -fno-rtti")

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)

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

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# CMake选项 动态库编译
option(BUILD_SHARED_LIBS "option for build" ON)
if(BUILD_SHARED_LIBS)
    set(BUILD_LIBS_TYPE SHARED)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
else()
    set(BUILD_LIBS_TYPE STATIC)
endif()

# CMake选项 MSVC MD编译
if(MSVC)
    option(BUILD_MSVC_MD "option for build" ON)
    set(COMPILER_FLAGS
        CMAKE_C_FLAGS CMAKE_CXX_FLAGS
        CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(BUILD_MSVC_MD)
        foreach(COMPILER_FLAG ${COMPILER_FLAGS})
            string(REPLACE "/MT" "/MD" ${COMPILER_FLAG} "${${COMPILER_FLAG}}")
        endforeach()
    else()
        foreach(COMPILER_FLAG ${COMPILER_FLAGS})
            string(REPLACE "/MD" "/MT" ${COMPILER_FLAG} "${${COMPILER_FLAG}}")
        endforeach()
    endif(BUILD_MSVC_MD)
endif(MSVC)

add_subdirectory(modules)
#add_subdirectory(3rd)

# CMake选项 app编译
option(BUILD_APP "option for build" ON)
if(BUILD_APP)
    add_subdirectory(app)
endif()

# CMake选项 UT测试
option(BUILD_TEST "option for build" OFF)
if(BUILD_TEST)
    add_subdirectory(tests)
endif()

