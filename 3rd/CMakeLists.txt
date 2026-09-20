

if(BUILD_TESTS)
    add_subdirectory(googletest)
endif()

add_subdirectory(json)
add_subdirectory(spdlog)

# Performance benchmark dependencies
if(BUILD_PERFORMANCE)
    add_subdirectory(benchmark)
    add_subdirectory(proxy)
endif()

# Optional Qt-based components
if(BUILD_APP)
    find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets PrintSupport)
    if(QT_FOUND)
        message(STATUS "Qt found, building available Qt components")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/QCustomPlot/CMakeLists.txt")
            add_subdirectory(QCustomPlot)
        endif()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qwindowkit/CMakeLists.txt")
            add_subdirectory(qwindowkit)
        endif()
    else()
        message(STATUS "Qt not found, skipping QCustomPlot and QWindowKit")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spdlog PRIVATE -w)
endif()
