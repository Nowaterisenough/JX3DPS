cmake_minimum_required(VERSION 3.11)

project(PlayerStatePanel LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/player_state_panel.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/player_state_panel.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER controls
)
