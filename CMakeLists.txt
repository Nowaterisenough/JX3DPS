cmake_minimum_required(VERSION 3.11)

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
option(BUILD_APP "option for build" OFF)

# CMake选项 UT测试
option(BUILD_TESTS "option for build" ON)

add_subdirectory(3rd)
add_subdirectory(src)
add_subdirectory(modules)

add_definitions(-DEXPORT_JX3DPS)
add_definitions(-DIMPORT_STATIC_THREAD_POOL_LIB)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/modules)
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h* ${CMAKE_SOURCE_DIR}/include/*.h*)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c*)

add_library(${PROJECT_NAME} STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ThreadPool
    nlohmann_json
    spdlog
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_NAME}/bin
    BUNDLE DESTINATION ${PROJECT_NAME}/bin
)

set(HEADERS ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}.h)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION {PROJECT_VERSION_MAJOR}
    FOLDER JX3DPS
)

install(TARGETS ${PROJECT_NAME}
    ThreadPool
    nlohmann_json
    spdlog
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in # <input>
    ${PROJECT_NAME}Config.cmake # <output>
    INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
)

if(NOT BUILD_SHARED_LIBS)
    file(APPEND
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        "add_definitions(-DIMPORT_STATIC_JX3DPS_LIB)\n"
    )
endif(NOT BUILD_SHARED_LIBS)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)

if(BUILD_APP)
    add_subdirectory(app)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()