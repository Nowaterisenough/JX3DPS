# The core uses the vendored nlohmann single header. Other dependencies are
# built only by their consumers.
if(BUILD_PERFORMANCE)
    add_subdirectory(benchmark)
    add_subdirectory(proxy)
endif()

if(BUILD_APP AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qwindowkit/CMakeLists.txt")
    find_package(Qt6 QUIET COMPONENTS Widgets)
    if(Qt6_FOUND)
        # Keep the window libraries alongside the packaged desktop binaries.
        set(QWINDOWKIT_BUILD_RUNTIME_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        if(NOT WIN32 AND CMAKE_LIBRARY_OUTPUT_DIRECTORY)
            set(QWINDOWKIT_BUILD_LIBRARY_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
        endif()
        add_subdirectory(qwindowkit)
    endif()
endif()
