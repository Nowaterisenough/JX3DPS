cmake_minimum_required(VERSION 3.16)

project(JX3DPSUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DEXPORT_JX3DPSUI)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64")
# list(APPEND CMAKE_TOOLCHAIN_FILE "C:/Qt/6.5.0/msvc2022_64_static/lib/cmake/Qt6/qt.toolchain.cmake")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# qt_standard_project_setup()
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c*)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(${PROJECT_NAME} SHARED
        MANUAL_FINALIZATION
        ${HEADER_FILES}
        ${SOURCE_FILES}
    )
else()
    add_library(${PROJECT_NAME} SHARED
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${RESOURCES}
    )
endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    JX3DPS
    ThreadPool
    spdlog
    nlohmann_json
)

if(NOT __APPLE__)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
    )
endif()

get_target_property(QT_TYPE Qt${QT_VERSION_MAJOR}::Widgets TYPE)

if(QT_TYPE STREQUAL "STATIC_LIBRARY")
    message(STATUS "QT_TYPE: STATIC_LIBRARY")
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    message(STATUS "QT_TYPE: SHARED_LIBRARY")
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "my.example.com"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE # TRUE 关闭命令行窗口
    FOLDER apps
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_NAME}/bin
    BUNDLE DESTINATION ${PROJECT_NAME}/bin
)

if(BUILD_SHARED_LIBS AND WIN32)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${DEPLOYQT_EXECUTABLE}
        --verbose 1
        --no-svg

        # --no-angle
        --no-opengl
        --no-opengl-sw
        --no-compiler-runtime
        --no-system-d3d-compiler
        --no-translations
        $<TARGET_FILE:${PROJECT_NAME}>

        # --$<LOWER_CASE:$<CONFIG>> --qmldir ${CMAKE_SOURCE_DIR}
    )
elseif(NOT BUILD_SHARED_LIBS)
    add_definitions(
        -DIMPORT_STATIC_THREAD_POOL_LIB
        -DIMPORT_STATIC_JX3DPSUI_LIB
    )
endif()