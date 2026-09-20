if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

if(NOT EXISTS "${APP_EXECUTABLE}")
    message(FATAL_ERROR "Desktop executable is missing: ${APP_EXECUTABLE}")
endif()

get_filename_component(_app_directory "${APP_EXECUTABLE}" DIRECTORY)
set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${OBJDUMP_EXECUTABLE}")

# MSYS2 Qt depends on non-Qt libraries which windeployqt does not copy.
file(GLOB_RECURSE _desktop_libraries "${_app_directory}/*.dll")
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${APP_EXECUTABLE}"
    LIBRARIES ${_desktop_libraries}
    DIRECTORIES "${_app_directory}" "${QT_BIN_DIRECTORY}" "${COMPILER_BIN_DIRECTORY}"
    PRE_EXCLUDE_REGEXES "^[Aa][Pp][Ii]-" "^[Ee][Xx][Tt]-"
    POST_EXCLUDE_REGEXES ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\].*"
    RESOLVED_DEPENDENCIES_VAR _resolved
    UNRESOLVED_DEPENDENCIES_VAR _unresolved
)
if(_unresolved)
    message(FATAL_ERROR "Unresolved desktop runtime dependencies: ${_unresolved}")
endif()
foreach(_dependency IN LISTS _resolved)
    get_filename_component(_source_directory "${_dependency}" DIRECTORY)
    if(NOT _source_directory STREQUAL _app_directory)
        file(COPY "${_dependency}" DESTINATION "${_app_directory}")
    endif()
endforeach()
