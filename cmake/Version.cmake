function(VERSION)
    # 直接设置版本号，避免复杂的正则解析
    set(VERSION_MAJOR 5)
    set(VERSION_MINOR 41)  # TAI_JI_MI_LU

    # 尝试从Git获取信息
    find_package(Git QUIET)

    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if(GIT_BRANCH)
            set(VERSION_BRANCH ${GIT_BRANCH} PARENT_SCOPE)
        else()
            set(VERSION_BRANCH "local" PARENT_SCOPE)
        endif()
    else()
        set(VERSION_BRANCH "local" PARENT_SCOPE)
    endif()

    # 使用当前时间戳作为版本号
    string(TIMESTAMP VERSION_PATCH "%y%m%d")
    string(TIMESTAMP VERSION_TWEAK "%H%M")

    set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}" PARENT_SCOPE)
    set(VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)
    set(VERSION_TWEAK ${VERSION_TWEAK} PARENT_SCOPE)
endfunction()
