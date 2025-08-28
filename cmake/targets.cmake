# cmake/targets.cmake
# 按目标启用告警、优化、可选 AVX2、LTO。避免全局污染第三方与示例。

# 告警配置
function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
        # 可选：禁用部分 noisy 警告
        # target_compile_options(${target} PRIVATE /wd4996)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# 优化级别：仅在 Release/RelWithDebInfo 下启用较高优化
function(enable_project_optimizations target)
    get_target_property(_tgt_type ${target} TYPE)
    # 对于 INTERFACE 库无需设置
    if(_tgt_type STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    if(CMAKE_CONFIGURATION_TYPES) # 多配置生成器（MSVC/Xcode）
        # 无需在这里判断配置，交给生成器区分；可以添加通用建议
        if(MSVC)
            # 对 MSVC，Release 一般默认 /O2；不重复设置
        else()
            target_compile_options(${target} PRIVATE "$<$<CONFIG:Release>:-O3>" "$<$<CONFIG:RelWithDebInfo>:-O3>")
        endif()
    else() # 单配置（如 Ninja/Makefiles）
        if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
            if(MSVC)
                # 通常 Release 默认 /O2
            else()
                target_compile_options(${target} PRIVATE -O3)
            endif()
        endif()
    endif()
endfunction()

# AVX2（按目标，可选）
function(enable_avx2_if_supported target)
    if(NOT (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64"))
        return()
    endif()
    include(CheckCXXCompilerFlag)
    if(MSVC)
        check_cxx_compiler_flag("/arch:AVX2" HAVE_AVX2_MSC)
        if(HAVE_AVX2_MSC)
            target_compile_options(${target} PRIVATE /arch:AVX2)
        endif()
    else()
        check_cxx_compiler_flag("-mavx2" HAVE_AVX2_GCC)
        if(HAVE_AVX2_GCC)
            target_compile_options(${target} PRIVATE -mavx2)
        endif()
    endif()
endfunction()

# LTO（按目标，若支持）
function(enable_lto_if_supported target)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_msg)
    if(_ipo_supported)
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO/LTO not supported: ${_ipo_msg}")
    endif()
endfunction()