cmake_minimum_required(VERSION 3.11)

# 如果正在为 WebAssembly 构建，设置工具链文件
if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{EMSCRIPTEN})
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/emscripten.cmake"
        CACHE FILEPATH "CMake toolchain file" FORCE)
endif()

project(JX3DPS LANGUAGES CXX)

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

# CMake选项 app编译
option(BUILD_APP "option for build" ON)

# CMake选项 UT测试
option(BUILD_TESTS "option for build" OFF)

add_subdirectory(3rd)
add_subdirectory(src)
add_subdirectory(modules)

# Resources 需要 Qt，仅在 BUILD_APP 时构建
if(BUILD_APP)
    add_subdirectory(resources)
endif()

set(BUILD_SHARED_LIBS true)

add_definitions(-DEXPORT_JX3DPS)
# add_definitions(-DIMPORT_STATIC_THREAD_POOL_LIB)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/modules)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h* ${CMAKE_SOURCE_DIR}/include/*.h*)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c*)

add_library(${PROJECT_NAME} STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    thread_pool
    nlohmann_json
    spdlog
)

# WebAssembly 特定配置
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "配置 WebAssembly 构建")

    # 导出 JX3DPS 函数供 WebAssembly 使用
    target_link_options(${PROJECT_NAME} PRIVATE
        -sEXPORTED_FUNCTIONS=_jx3dps_version,_jx3dps_jx3_version,_jx3dps_simulate,_jx3dps_get_result,_jx3dps_get_progress,_jx3dps_debug,_jx3dps_debugger_step_in,_jx3dps_debugger_step_over,_jx3dps_debugger_continue,_jx3dps_debugger_set_breakpoint,_malloc,_free
        -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToUTF8,UTF8ToString
        -sALLOW_MEMORY_GROWTH=1
        -sMODULARIZE=1
        -sEXPORT_NAME=JX3DPS
        -sINITIAL_MEMORY=16777216
        -sNO_EXIT_RUNTIME=1
    )

    # 构建为可执行文件以生成 WASM
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".js")

    # 复制 WebAssembly 产物到 build/out 目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/out/jx3dps.js
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
            ${CMAKE_BINARY_DIR}/out/jx3dps.wasm
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/apps/web/index.html
            ${CMAKE_BINARY_DIR}/out/index.html
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/apps/web/coi-serviceworker.js
            ${CMAKE_BINARY_DIR}/out/coi-serviceworker.js
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_NAME}/bin
    BUNDLE DESTINATION ${PROJECT_NAME}/bin
)

set(HEADERS ${CMAKE_SOURCE_DIR}/include/jx3dps.h)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    FOLDER JX3DPS
)

# Temporarily disable install exports to fix build issues
# install(TARGETS ${PROJECT_NAME}
#     EXPORT ${PROJECT_NAME}Targets
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     PUBLIC_HEADER DESTINATION include
# )

# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
#     VERSION ${PACKAGE_VERSION}
#     COMPATIBILITY AnyNewerVersion
# )

# install(EXPORT ${PROJECT_NAME}Targets
#     FILE ${PROJECT_NAME}Targets.cmake
#     DESTINATION "lib/cmake/${PROJECT_NAME}"
# )

# configure_package_config_file(
#     ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in # <input>
#     ${PROJECT_NAME}Config.cmake # <output>
#     INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
# )

# if(NOT BUILD_SHARED_LIBS)
#     file(APPEND
#         ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#         "add_definitions(-DIMPORT_STATIC_JX3DPS_LIB)\n"
#     )
# endif(NOT BUILD_SHARED_LIBS)

# install(FILES
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
#     DESTINATION "lib/cmake/${PROJECT_NAME}"
# )

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
    add_subdirectory(tests)
endif()

# CMake选项 性能测试样例
option(BUILD_PERFORMANCE "Build performance benchmarks" OFF)

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
message(STATUS "  thread_pool:     ENABLED")
message(STATUS "  JSON (nlohmann): ENABLED")
message(STATUS "  spdlog:          ENABLED")
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