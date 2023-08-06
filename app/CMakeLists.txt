cmake_minimum_required(VERSION 3.11)

project(JX3DPSSimulator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

if(NOT BUILD_SHARED_LIBS)
    add_definitions(
        -DIMPORT_STATIC_THREAD_POOL_LIB
        -DIMPORT_STATIC_JX3DPS_LIB
        -DIMPORT_STATIC_WIDGET_LIB
    )
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/modules/Widgets)

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c*)

file(GLOB RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/*.qrc
    ${CMAKE_SOURCE_DIR}/resources/*.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${RESOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${RESOURCES}
    )
endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    JX3DPS
    ThreadPool
    spdlog
    nlohmann_json
    Qt${QT_VERSION_MAJOR}::Widgets
    Widget
)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /NODEFAULTLIB:msvcrt.lib)
endif()

set(APP_NAME "JX3DPS Simulator")

add_definitions(-DAPP_NAME="${APP_NAME}")

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
    WIN32_EXECUTABLE TRUE # TRUE 关闭命令行窗口
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
        --no-pdf
        --no-network
        --no-opengl
        --no-opengl-sw
        --no-compiler-runtime
        --no-system-d3d-compiler
        --no-translations
        --no-virtualkeyboard
        --$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>
        $<TARGET_FILE:${PROJECT_NAME}>
    )

    add_custom_command(
        TARGET ${PROJECT_NAME} # 指定编译的目标名
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/config/config.json # 要复制的文件路径
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/config.json # 目标路径
    )
endif()

