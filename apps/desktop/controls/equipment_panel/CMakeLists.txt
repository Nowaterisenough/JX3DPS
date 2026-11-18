cmake_minimum_required(VERSION 3.11)

project(EquipmentPanel LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_library(${PROJECT_NAME} SHARED
    equipment_panel.h
    equipment_panel.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE EQUIPMENT_PANEL_BUILD)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER controls)
