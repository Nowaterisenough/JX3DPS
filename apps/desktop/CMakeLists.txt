cmake_minimum_required(VERSION 3.11)

project(JX3DPSDesktop LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Version.cmake)
VERSION()

# 设置项目名称&版本
set(VERSION ${PROJECT_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

# Add controls subdirectory
add_subdirectory(controls/frameless)
add_subdirectory(controls/theme)
add_subdirectory(controls/code_editor)
add_subdirectory(controls/timeline)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data_generator.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data_generator.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${CMAKE_SOURCE_DIR}/resources/win32app_icon.rc
    )
else()
    add_executable(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${CMAKE_SOURCE_DIR}/resources/win32app_icon.rc
    )
endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Frameless
    Theme
    CodeEditor
    Timeline
    Resources
)

set(APP_NAME "JX3DPS Desktop")

if(NOT __APPLE__)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${APP_NAME}
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "my.example.com"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    FOLDER app
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_NAME}/bin
    BUNDLE DESTINATION ${PROJECT_NAME}/bin
)

if(WIN32)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${DEPLOYQT_EXECUTABLE}
        --verbose 1
        --no-svg
        --no-network
        --no-opengl-sw
        --no-compiler-runtime
        --no-system-d3d-compiler
        --no-translations
        --$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>
        $<TARGET_FILE:${PROJECT_NAME}>
    )
endif()
