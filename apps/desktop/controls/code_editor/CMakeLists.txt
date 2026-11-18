# Code Editor Component

cmake_minimum_required(VERSION 3.11)

project(CodeEditor LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CODE_EDITOR_SOURCES
    code_editor.cpp
    debug_toolbar.cpp
    simulation_toolbar.cpp
)

set(CODE_EDITOR_HEADERS
    code_editor.h
    debug_toolbar.h
    simulation_toolbar.h
)

add_library(${PROJECT_NAME} STATIC
    ${CODE_EDITOR_SOURCES}
    ${CODE_EDITOR_HEADERS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Resources
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/resources
)
