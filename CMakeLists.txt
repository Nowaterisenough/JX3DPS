cmake_minimum_required(VERSION 3.25)

include(${CMAKE_SOURCE_DIR}/cmake/version.cmake)
version()

project(JX3DPS VERSION ${PROJECT_VERSION})

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
get_version_date()
print_table(
    "PROJECT" ${PROJECT_NAME}
    "VERSION" ${PROJECT_VERSION}
    "BRANCH" ${VERSION_BRANCH}
    "DATE" ${VERSION_DATE}
)

message(STATUS "VERSION=${PROJECT_VERSION}")

add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_TWEAK=${PROJECT_VERSION_TWEAK})
add_definitions(-DVERSION_BRANCH=${VERSION_BRANCH})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    add_compile_options(-O3 -Wall -mavx2)
else()
    add_compile_options(-O3 -Wall)
endif()

set(BUILD_TESTING OFF CACHE BOOL "Build tests")
option(BUILD_EXAMPLE "option for build" ON)
option(BUILD_PERFORMANCE "option for build" ON)

add_subdirectory(3rd)

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

target_link_libraries(${PROJECT_NAME} PUBLIC
    nlohmann_json
    spdlog
    msft_proxy4
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    PREFIX ""
    OUTPUT_NAME ${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_PERFORMANCE)
    add_subdirectory(performance)
endif()

