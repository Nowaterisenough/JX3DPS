cmake_minimum_required(VERSION 3.16)

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
add_subdirectory(controls/player_state_panel)
add_subdirectory(controls/equipment_panel)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/game_icons.h
    ${CMAKE_CURRENT_SOURCE_DIR}/combat_history.h
    ${CMAKE_CURRENT_SOURCE_DIR}/history_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/traced_rules.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debug_session.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debug_simulator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/batch_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/combat_event.h
    ${CMAKE_CURRENT_SOURCE_DIR}/buff_trace.h
    ${CMAKE_CURRENT_SOURCE_DIR}/buff_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_step.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_trace_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_search_bar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation_result.h
    ${CMAKE_CURRENT_SOURCE_DIR}/results_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation_options.h
    ${CMAKE_CURRENT_SOURCE_DIR}/workbench.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/game_icons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/combat_history.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history_panel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/desktop_data.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/debug_session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug_simulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/batch_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buff_trace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buff_panel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_trace_panel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/macro_search_bar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/results_panel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation_options.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/workbench.cpp
)

add_library(DesktopWorkbench STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(DesktopWorkbench PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(DesktopWorkbench PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Frameless Theme CodeEditor Timeline PlayerStatePanel Resources EquipmentPanel Qt${QT_VERSION_MAJOR}::Network)
target_compile_features(DesktopWorkbench PUBLIC cxx_std_20)
if(JX3DPS_DESKTOP_TESTS)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    add_executable(desktop_tests ${CMAKE_SOURCE_DIR}/tests/desktop/desktop_tests.cpp)
    target_link_libraries(desktop_tests PRIVATE DesktopWorkbench Qt${QT_VERSION_MAJOR}::Test)
    add_test(NAME desktop_tests COMMAND desktop_tests -platform offscreen)
    set_tests_properties(desktop_tests PROPERTIES TIMEOUT 60)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_SOURCE_DIR}/resources/win32app_icon.rc
    )
else()
    add_executable(${PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_SOURCE_DIR}/resources/win32app_icon.rc
    )
endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Frameless
    Theme
    CodeEditor
    Timeline
    PlayerStatePanel
    EquipmentPanel
    Resources
    DesktopWorkbench
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/3rd/spdlog/include
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

# Keep the last synchronized 魔盒 snapshot beside the desktop binary. The
# application never downloads data during simulation; it only refreshes this
# file through tools/sync_jx3box_data.ps1.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/jx3box
    DESTINATION ${PROJECT_NAME}/data
)

if(WIN32)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin_dir}")
    if(NOT DEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt is required to deploy the desktop application")
    endif()

    get_filename_component(_compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(_runtime_scan_args)
    if(MINGW)
        list(APPEND _runtime_scan_args
            "-DCMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM=windows+pe"
            "-DCMAKE_GET_RUNTIME_DEPENDENCIES_TOOL=objdump"
            "-DCMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND=${CMAKE_OBJDUMP}"
        )
    endif()
    set(_runtime_deploy_script "${CMAKE_SOURCE_DIR}/cmake/DeployWindowsRuntime.cmake")
    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_DEPENDS "${_runtime_deploy_script}")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "${DEPLOYQT_EXECUTABLE}"
        --verbose 1
        --no-svg
        --no-opengl-sw
        --no-compiler-runtime
        --no-system-d3d-compiler
        --translations zh_CN
        "$<IF:$<CONFIG:Debug>,--debug,--release>"
        "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}"
            "-DRUNTIME_APPLICATION=$<TARGET_FILE:${PROJECT_NAME}>"
            "-DRUNTIME_QT_BIN_DIR=${_qt_bin_dir}"
            "-DRUNTIME_COMPILER_BIN_DIR=${_compiler_bin_dir}"
            ${_runtime_scan_args}
            -P "${_runtime_deploy_script}"
        VERBATIM
    )
endif()
