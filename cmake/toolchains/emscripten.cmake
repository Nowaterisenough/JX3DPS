# Emscripten toolchain for CMake
# This file configures CMake to use Emscripten compilers (emcc/em++) for WebAssembly compilation
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/emscripten.cmake ..

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_PROCESSOR x86)

# Find Emscripten compiler executables
find_program(CMAKE_C_COMPILER 
    NAMES emcc.bat emcc
    HINTS 
        ENV EMSDK
        ENV PATH
    PATHS 
        $ENV{EMSDK}/upstream/emscripten
        $ENV{EMSDK}/fastcomp/emscripten
    DOC "Emscripten C compiler"
)

find_program(CMAKE_CXX_COMPILER 
    NAMES em++.bat em++
    HINTS 
        ENV EMSDK
        ENV PATH
    PATHS 
        $ENV{EMSDK}/upstream/emscripten
        $ENV{EMSDK}/fastcomp/emscripten
    DOC "Emscripten C++ compiler"
)

find_program(CMAKE_AR 
    NAMES emar.bat emar
    HINTS 
        ENV EMSDK
        ENV PATH
    PATHS 
        $ENV{EMSDK}/upstream/emscripten
        $ENV{EMSDK}/fastcomp/emscripten
    DOC "Emscripten archiver"
)

find_program(CMAKE_RANLIB 
    NAMES emranlib.bat emranlib
    HINTS 
        ENV EMSDK
        ENV PATH
    PATHS 
        $ENV{EMSDK}/upstream/emscripten
        $ENV{EMSDK}/fastcomp/emscripten
    DOC "Emscripten ranlib"
)

# Validate that compilers were found
if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "Could not find emcc. Please ensure Emscripten SDK is installed and in PATH.")
endif()

if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Could not find em++. Please ensure Emscripten SDK is installed and in PATH.")
endif()

message(STATUS "Using Emscripten C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using Emscripten C++ compiler: ${CMAKE_CXX_COMPILER}")

# Tell CMake that this is a cross-compilation environment
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_SIZEOF_VOID_P 4)

# Cross-compilation search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Emscripten-specific flags
set(EMSCRIPTEN_COMMON_FLAGS "-s USE_PTHREADS=1 -pthread")
set(CMAKE_C_FLAGS_INIT "${EMSCRIPTEN_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${EMSCRIPTEN_COMMON_FLAGS}")

# Build type specific flags
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -gsource-map")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -gsource-map")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Emscripten linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${EMSCRIPTEN_COMMON_FLAGS}")

# Set output file extensions
set(CMAKE_EXECUTABLE_SUFFIX ".js")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

# Disable compiler checks (Emscripten doesn't support typical executable formats)
set(CMAKE_C_COMPILER_ID_RUN FALSE)
set(CMAKE_CXX_COMPILER_ID_RUN FALSE)