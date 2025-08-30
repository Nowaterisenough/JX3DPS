cmake_minimum_required(VERSION 3.25)

# -------------------- Version & project --------------------
include(${CMAKE_SOURCE_DIR}/cmake/version.cmake)
version()

project(JX3DPS VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# -------------------- Build options (top) --------------------
option(BUILD_EXAMPLES         "Build examples"                    ON)
option(BUILD_TESTING          "Build and run tests"               OFF)
option(BUILD_APPS_DESKTOP     "Build desktop apps"                ON)
option(BUILD_APPS_WEB         "Build web app(s)"                  ON)
option(BUILD_PLATFORMS        "Build platform adapters"           ON)
option(BUILD_TOOLS            "Build auxiliary tools"             ON)
option(BUILD_PERF_BENCHMARKS  "Build perf/benchmarks"             ON)
option(BUILD_PERF_EXPERIMENTS "Build perf/experiments"            ON)

# 构建特性
option(ENABLE_AVX2            "Enable AVX2 for supported compilers (per-target)" ON)
option(ENABLE_LTO             "Enable link-time optimization (per-target)"        OFF)

# -------------------- Utilities & target helpers --------------------
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/targets.cmake)

# -------------------- Pretty banner --------------------
get_version_date()
print_table(
    "PROJECT" ${PROJECT_NAME}
    "VERSION" ${PROJECT_VERSION}
    "BRANCH" ${VERSION_BRANCH}
    "DATE" ${VERSION_DATE}
)
message(STATUS "VERSION=${PROJECT_VERSION}")

# -------------------- Global setup (non-invasive) --------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

# -------------------- Third-party --------------------
add_subdirectory(3rd)

# -------------------- Main library target --------------------
file(GLOB SOURCES "src/*.c*" "src/*.h*")
add_definitions(-DEXPORT_JX3DPS)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mount
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        nlohmann_json
        spdlog
        msft_proxy4
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    PREFIX ""
    OUTPUT_NAME ${PROJECT_NAME}
)

enable_project_warnings(${PROJECT_NAME})
enable_project_optimizations(${PROJECT_NAME})
if(ENABLE_AVX2)
    enable_avx2_if_supported(${PROJECT_NAME})
endif()
if(ENABLE_LTO)
    enable_lto_if_supported(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# -------------------- Conditional subdirs --------------------
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_PERF_BENCHMARKS)
    add_subdirectory(perf/benchmarks)
endif()

if(BUILD_PERF_EXPERIMENTS)
    add_subdirectory(perf/experiments)
endif()

if(BUILD_APPS_DESKTOP)
    add_subdirectory(apps/desktop)
endif()

if(BUILD_APPS_WEB)
    add_subdirectory(apps/web)
endif()

if(BUILD_PLATFORMS)
    add_subdirectory(platforms)
endif()

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()