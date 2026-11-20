# The core uses the vendored nlohmann single header. Other dependencies are
# built only by their consumers.
if(BUILD_PERFORMANCE)
    add_subdirectory(benchmark)
    add_subdirectory(proxy)
endif()

if(BUILD_APP AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qwindowkit/CMakeLists.txt")
    find_package(Qt6 QUIET COMPONENTS Widgets)
    if(Qt6_FOUND)
        add_subdirectory(qwindowkit)
    endif()
endif()
