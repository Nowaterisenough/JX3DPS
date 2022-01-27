cmake_minimum_required(VERSION 3.5)

# 时间戳
string(TIMESTAMP TIMESTAMP_VERSION "%y%m%d.%H%M")

set(JX3_VERSION 29)

# 定义项目名称
set(PROJECT_NAME JX3DPS)

# 设置项目名称&版本
project(${PROJECT_NAME})

# 定义VERSION & APP_NAME
add_definitions(-DTIMESTAMP_VERSION=${TIMESTAMP_VERSION})

set(PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

function(include_sub_directories_recursively root_dir)
    if (IS_DIRECTORY ${root_dir})               # 当前路径是一个目录吗，是的话就加入到包含目录
        include_directories(${root_dir})
    endif()

    file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*) # 获得当前目录下的所有文件，让如ALL_SUB列表中
    foreach(sub ${ALL_SUB})
        if (IS_DIRECTORY ${root_dir}/${sub})
            include_sub_directories_recursively(${root_dir}/${sub}) # 对子目录递归调用，包含
        endif()
    endforeach()
endfunction()

include_sub_directories_recursively(${PROJECT_ROOT_PATH}/include) # 对子目录递归调用，包含


# CMake选项 动态库编译
option(BUILD_SHARED_LIBS "option for build" ON)
if(BUILD_SHARED_LIBS)
    set(BUILD_LIBS_TYPE SHARED)
else()
    set(BUILD_LIBS_TYPE STATIC)
endif()

# CMake选项 MSVC MD编译
option(BUILD_MSVC_MD "option for build" ON)
if(MSVC)
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
else()
endif(MSVC)

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

add_subdirectory(src)