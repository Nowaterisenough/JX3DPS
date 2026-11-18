cmake_minimum_required(VERSION 3.12)

project(JX3DPS_Example LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    add_compile_options(-O3 -Wall -mavx2)
else()
    add_compile_options(-O3 -Wall)
endif()


file(GLOB SOURCES "*.c*" "*.h*")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    msft_proxy
    JX3DPS
)
