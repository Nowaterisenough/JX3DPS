# The core uses the vendored nlohmann single header. Other dependencies are
# built only by their consumers.
if(BUILD_PERFORMANCE)
    set(GOOGLETEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/googletest" CACHE PATH "GoogleTest sources for Google Benchmark")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/proxy/include/proxy/proxy.h")
        message(FATAL_ERROR "Initialize the benchmark dependency with: git submodule update --init 3rd/proxy")
    endif()
    add_library(jx3dps_proxy INTERFACE)
    target_include_directories(jx3dps_proxy SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/proxy/include")
    target_compile_features(jx3dps_proxy INTERFACE cxx_std_20)
    add_subdirectory(benchmark)
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
