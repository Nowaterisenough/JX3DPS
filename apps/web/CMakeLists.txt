# Configure from the repository root with emcmake cmake.
add_executable(jx3dps_web wasm/module.cpp)
target_link_libraries(jx3dps_web PRIVATE JX3DPS)
target_compile_features(jx3dps_web PRIVATE cxx_std_23)
target_compile_options(jx3dps_web PRIVATE -fexceptions)
target_link_options(jx3dps_web PRIVATE
    --no-entry -fexceptions
    -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=worker,node
    -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576 -sFILESYSTEM=0
    "-sEXPORTED_FUNCTIONS=_jx3dps_create,_jx3dps_destroy,_jx3dps_call,_jx3dps_abi_version" "-sEXPORTED_RUNTIME_METHODS=ccall")
set(site "${CMAKE_BINARY_DIR}/out")
set_target_properties(jx3dps_web PROPERTIES OUTPUT_NAME jx3dps SUFFIX ".mjs" RUNTIME_OUTPUT_DIRECTORY "${site}")

# Copy inputs on every build (including frontend-only edits) without touching unchanged files.
add_custom_target(web_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${site}/ui"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/index.html" "${site}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_CURRENT_SOURCE_DIR}/ui" "${site}/ui"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/data/images/JX3/Icons" "${site}/icons"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/apps/desktop/data/icons.json" "${site}/icons.json"
    VERBATIM)
add_dependencies(jx3dps_web web_assets)
