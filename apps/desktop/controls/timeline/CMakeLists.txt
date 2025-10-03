# Timeline Component

cmake_minimum_required(VERSION 3.11)

project(Timeline LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(TIMELINE_SOURCES
    timeline.cpp
)

set(TIMELINE_HEADERS
    timeline.h
)

add_library(${PROJECT_NAME} STATIC
    ${TIMELINE_SOURCES}
    ${TIMELINE_HEADERS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
