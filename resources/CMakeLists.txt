cmake_minimum_required(VERSION 3.11)

project(Resources LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Collect source files
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

# Create shared library
add_library(${PROJECT_NAME} SHARED
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${QRC_FILES}
)

# Add export definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCES_API_EXPORT
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Widgets
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER libraries
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    AUTOUIC TRUE
    AUTOMOC TRUE
    AUTORCC TRUE
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
