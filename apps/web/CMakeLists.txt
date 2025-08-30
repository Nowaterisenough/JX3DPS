cmake_minimum_required(VERSION 3.12)

project(JX3DPS_Web LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glob for source files
file(GLOB WEB_SOURCES "*.cpp" "*.cc" "*.cxx" "*.c")
file(GLOB WEB_HEADERS "*.hpp" "*.h" "*.hxx")

# Only create targets if there are source files
if(WEB_SOURCES)
    add_executable(${PROJECT_NAME} ${WEB_SOURCES} ${WEB_HEADERS})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        JX3DPS
        nlohmann_json
        spdlog
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_EXTENSIONS OFF
    )
    
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
else()
    message(STATUS "No web app source files found in apps/web")
endif()