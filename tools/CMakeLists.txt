cmake_minimum_required(VERSION 3.12)

project(JX3DPS_Tools LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glob for source files
file(GLOB TOOL_SOURCES "*.cpp" "*.cc" "*.cxx" "*.c")
file(GLOB TOOL_HEADERS "*.hpp" "*.h" "*.hxx")

# Only create targets if there are source files
if(TOOL_SOURCES)
    foreach(TOOL_SRC ${TOOL_SOURCES})
        get_filename_component(TOOL_NAME ${TOOL_SRC} NAME_WE)
        add_executable(${TOOL_NAME} ${TOOL_SRC})
        
        target_link_libraries(${TOOL_NAME} PRIVATE
            JX3DPS
            nlohmann_json
            spdlog
        )
        
        set_target_properties(${TOOL_NAME} PROPERTIES
            CXX_EXTENSIONS OFF
        )
        
        install(TARGETS ${TOOL_NAME}
            RUNTIME DESTINATION bin
        )
    endforeach()
else()
    message(STATUS "No tool source files found in tools")
endif()