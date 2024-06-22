add_subdirectory(json)
add_subdirectory(proxy)
add_subdirectory(spdlog)

if(BUILD_TESTING)
    add_subdirectory(googletest)
endif()

if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

# add_subdirectory(QCustomPlot)
