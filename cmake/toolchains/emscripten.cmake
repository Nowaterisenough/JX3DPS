# Compatibility for builds that select this file explicitly.
# emcmake selects this SDK-provided toolchain automatically.
if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Activate the Emscripten SDK before configuring a Web build")
endif()
include("$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
