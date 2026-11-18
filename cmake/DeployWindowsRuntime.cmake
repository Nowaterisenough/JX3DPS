cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

if(NOT EXISTS "${RUNTIME_APPLICATION}")
    message(FATAL_ERROR "Desktop executable not found: ${RUNTIME_APPLICATION}")
endif()

get_filename_component(_app_dir "${RUNTIME_APPLICATION}" DIRECTORY)

# windeployqt supplies Qt plugins, but MSYS2 builds also need their non-Qt
# dependencies (ICU, image codecs, font libraries and the compiler runtime).
file(GLOB_RECURSE _libraries "${_app_dir}/*.dll")
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${RUNTIME_APPLICATION}"
    LIBRARIES ${_libraries}
    DIRECTORIES "${_app_dir}" "${RUNTIME_QT_BIN_DIR}" "${RUNTIME_COMPILER_BIN_DIR}"
    RESOLVED_DEPENDENCIES_VAR _resolved
    UNRESOLVED_DEPENDENCIES_VAR _unresolved
    CONFLICTING_DEPENDENCIES_PREFIX _conflicts
    PRE_EXCLUDE_REGEXES "api-ms-.*" "ext-ms-.*"
    POST_EXCLUDE_REGEXES
        ".*[/\\\\][Ss][Yy][Ss][Tt][Ee][Mm]32[/\\\\].*"
        ".*[/\\\\][Ss][Yy][Ss][Ww][Oo][Ww]64[/\\\\].*"
)

if(_unresolved)
    message(FATAL_ERROR "Unresolved desktop runtime dependencies: ${_unresolved}")
endif()

# A dependency may exist both beside the executable and in the Qt toolchain.
# Accept identical copies, but never silently deploy incompatible versions.
foreach(_filename IN LISTS _conflicts_FILENAMES)
    set(_candidates "${_conflicts_${_filename}}")
    list(GET _candidates 0 _selected)
    file(SHA256 "${_selected}" _selected_hash)
    foreach(_candidate IN LISTS _candidates)
        file(SHA256 "${_candidate}" _candidate_hash)
        if(NOT _candidate_hash STREQUAL _selected_hash)
            message(FATAL_ERROR "Conflicting runtime versions for ${_filename}: ${_candidates}")
        endif()
    endforeach()
    list(APPEND _resolved "${_selected}")
endforeach()

set(_copied_count 0)
foreach(_dependency IN LISTS _resolved)
    get_filename_component(_dependency_dir "${_dependency}" DIRECTORY)
    if(_dependency_dir STREQUAL _app_dir)
        continue()
    endif()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_dependency}" "${_app_dir}"
        RESULT_VARIABLE _copy_result
    )
    if(NOT _copy_result EQUAL 0)
        message(FATAL_ERROR "Failed to deploy runtime dependency: ${_dependency}")
    endif()
    math(EXPR _copied_count "${_copied_count} + 1")
endforeach()
message(STATUS "Deployed ${_copied_count} runtime dependencies to ${_app_dir}")
