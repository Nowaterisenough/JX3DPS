# ============================================================================
# TaiXuJianYi Dynamic Library CMake Configuration
# ============================================================================

cmake_minimum_required(VERSION 3.11)

project(tai_xu_jian_yi VERSION 1.0.0 LANGUAGES CXX)

# C++20 Required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Library Type Selection
# ============================================================================
option(BUILD_SHARED_LIBS "Build shared library" ON)

if(BUILD_SHARED_LIBS)
    message(STATUS "Building TaiXuJianYi as SHARED library")
    set(LIBRARY_TYPE SHARED)
else()
    message(STATUS "Building TaiXuJianYi as STATIC library")
    set(LIBRARY_TYPE STATIC)
endif()

# ============================================================================
# Source Files
# ============================================================================

set(HEADER_FILES
    tai_xu_jian_yi.h
    tai_xu_jian_yi_ids.h
    tai_xu_jian_yi_buff.h
    tai_xu_jian_yi_skills.h
    tai_xu_jian_yi_export.h
)

set(SOURCE_FILES
    tai_xu_jian_yi.cpp
    tai_xu_jian_yi_skills.cpp
)

# ============================================================================
# Create Library Target
# ============================================================================

add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# ============================================================================
# Include Directories
# ============================================================================

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================================
# Compile Definitions
# ============================================================================

# Define export macro for Windows DLL
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            TXJY_BUILD_SHARED
            TXJY_EXPORT
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            TXJY_STATIC
    )
endif()

# ============================================================================
# Compiler Options
# ============================================================================

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4                 # Warning level 4
        /WX-                # Do not treat warnings as errors
        /permissive-        # Conformance mode
        /Zc:__cplusplus     # Enable updated __cplusplus macro
        /utf-8              # Set source and execution character sets to UTF-8
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# ============================================================================
# Link Libraries
# ============================================================================

# Link to core JX3DPS dependencies (if needed)
# Uncomment if the library needs these dependencies
# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
#         nlohmann_json
#         spdlog
# )

# ============================================================================
# Properties
# ============================================================================

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${HEADER_FILES}"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE ON
)

# Windows-specific properties
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS OFF  # We'll use explicit exports
    )
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/tai_xu_jian_yi
)

# Install export targets
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE JX3DPS::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Generate and install package config files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "TaiXuJianYi Configuration")
message(STATUS "========================================")
message(STATUS "Version:         ${PROJECT_VERSION}")
message(STATUS "Library Type:    ${LIBRARY_TYPE}")
message(STATUS "C++ Standard:    C++${CMAKE_CXX_STANDARD}")
message(STATUS "Install Prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")
