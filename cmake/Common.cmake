# common.cmake

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)

# 关闭转换精度损失警告
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wno-conversion)
    add_compile_options(-Wno-braced-scalar-init)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-shorten-64-to-32)
    add_compile_options(-Wno-braced-scalar-init)
elseif(MSVC)
    add_compile_options(/wd4244)
endif()

# set(CMAKE_BUILD_TYPE $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)

# set(CMAKE_BUILD_TYPE "Release")

# 输出结果
if(NOT BUILD_SHARED_LIBS)
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /utf-8 /MT /GR- /GS- /GL /Ot /Oi /Ob3 /fp:fast /Qpar /Qvec-report:2")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /utf-8 /MTd")
    endif()
else()
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /utf-8 /MD /GR- /GS- /GL /Ot /Oi /Ob3 /fp:fast /Qpar /Qvec-report:2")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /utf-8 /MDd")
    endif()
endif()



# install路径
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        if(WIN32)
            set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
        else()
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory" FORCE)
        endif()
    else()
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)