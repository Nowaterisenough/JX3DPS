cmake_minimum_required(VERSION 3.16)

project(JX3DPS LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置 C 语言编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /Ox /Ot /Oi /GL")

# 设置 C++ 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ox /Ot /Oi /GL")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DEXPORT_JX3DPS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c*)

if(NOT BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} STATIC
        ${HEADER_FILES}
        ${SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} SHARED
        ${HEADER_FILES}
        ${SOURCE_FILES})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    ThreadPool
    spdlog
    nlohmann_json
)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:/PROFILE>)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:RELWITHDEBINFO>:/PROFILE>)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:RELEASE>:/PROFILE>)
endif()

if(NOT __APPLE__)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_NAME}/bin
    BUNDLE DESTINATION ${PROJECT_NAME}/bin
)

if(NOT BUILD_SHARED_LIBS)
    add_definitions(
        -DIMPORT_STATIC_THREAD_POOL_LIB
        -DIMPORT_STATIC_JX3DPS_LIB
    )
endif()
