

if(BUILD_TESTS)
    add_subdirectory(googletest)
endif()

add_subdirectory(json)
add_subdirectory(spdlog)

# Optional Qt-based components
find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets PrintSupport)
if(QT_FOUND)
    message(STATUS "Qt found, building QCustomPlot")
    add_subdirectory(QCustomPlot)
else()
    message(STATUS "Qt not found, skipping QCustomPlot")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spdlog PRIVATE -w)
endif()