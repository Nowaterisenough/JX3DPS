cmake_minimum_required(VERSION 3.12)

project(JX3DPS_Platforms LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glob for source files
file(GLOB PLATFORM_SOURCES "*.cpp" "*.cc" "*.cxx" "*.c")
file(GLOB PLATFORM_HEADERS "*.hpp" "*.h" "*.hxx")

# Only create targets if there are source files
if(PLATFORM_SOURCES)
    add_library(${PROJECT_NAME} STATIC ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})
    
    target_link_libraries(${PROJECT_NAME} PUBLIC
        JX3DPS
        nlohmann_json
        spdlog
    )
    
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_EXTENSIONS OFF
    )
    
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
else()
    message(STATUS "No platform source files found in platforms")
endif()