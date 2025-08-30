cmake_minimum_required(VERSION 3.12)

project(JX3DPS_Benchmarks LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glob for source files
file(GLOB BENCHMARK_SOURCES "*.cpp" "*.cc" "*.cxx")

# Only create targets if there are source files
if(BENCHMARK_SOURCES)
    foreach(BENCHMARK_SRC ${BENCHMARK_SOURCES})
        get_filename_component(BENCHMARK_NAME ${BENCHMARK_SRC} NAME_WE)
        add_executable(${BENCHMARK_NAME} ${BENCHMARK_SRC})
        
        target_link_libraries(${BENCHMARK_NAME} PRIVATE
            JX3DPS
            benchmark::benchmark
            benchmark::benchmark_main
        )
        
        set_target_properties(${BENCHMARK_NAME} PROPERTIES
            CXX_EXTENSIONS OFF
        )
    endforeach()
else()
    message(STATUS "No benchmark source files found in perf/benchmarks")
endif()