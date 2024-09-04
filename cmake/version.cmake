function(version)
    file(READ "${CMAKE_SOURCE_DIR}/src/Version.h" FILE_TEXT)

    # Extract VERSION_MAJOR
    string(REGEX MATCH "VERSION_MAJOR[ ]+([0-9]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    set(VERSION_MAJOR ${CMAKE_MATCH_1})

    # Extract VERSION_MINOR
    string(REGEX MATCH "VERSION_MINOR[ ]+([a-zA-Z0-9_]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract minor version number from Version.h")
    endif()

    string(REGEX MATCH "${CMAKE_MATCH_1}[ ]+([a-zA-Z0-9_]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    string(REGEX MATCH "${CMAKE_MATCH_1}[ =]+([0-9]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    set(VERSION_MINOR ${CMAKE_MATCH_1})

    # Check if VERSION_MINOR is a variable reference (e.g., JX3_VERSION)
    string(REGEX MATCH "JX3_VERSION[ ]+([0-9]+)" _ "${FILE_TEXT}")

    if(CMAKE_MATCH_COUNT EQUAL 1)
        # If JX3_VERSION is defined, update VERSION_MINOR with its value
        set(VERSION_MINOR ${CMAKE_MATCH_1})
    endif()

    find_package(Git QUIET)
    execute_process(COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REPO_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )

    if(RET EQUAL "0")
        string(REGEX MATCH "Your branch is up to date with '(.*/(.*))'.[\r|\n]+nothing to commit, working tree clean" _ "${GIT_REPO_STATUS}")

        if(NOT CMAKE_MATCH_COUNT EQUAL 2)
            set(CUSTOM TRUE)
        else()
            set(REMOTE_BRANCH ${CMAKE_MATCH_1} PARENT_SCOPE)
            set(VERSION_BRANCH ${CMAKE_MATCH_2} PARENT_SCOPE)
            set(CUSTOM FALSE)
        endif()
    else()
        set(CUSTOM TRUE)
    endif()

    # 使用 TZ 环境变量设置北京时间
    set(ENV{TZ} "Asia/Shanghai")

    execute_process(COMMAND ${GIT_EXECUTABLE} log ${REMOTE_BRANCH} -1 --format=%cd --date=format-local:%m%d
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REPO_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(VERSION_PATCH ${GIT_REPO_DATE})

    execute_process(COMMAND ${GIT_EXECUTABLE} log ${REMOTE_BRANCH} -1 --date=format-local:%H%M --format=%cd
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REPO_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(VERSION_TWEAK ${GIT_REPO_TIME})

    if(CUSTOM)
        set(VERSION_BRANCH "local" PARENT_SCOPE)

        # 使用命令行工具获取北京时间
        if(WIN32)
            # Windows 系统使用 PowerShell
            execute_process(
                COMMAND powershell -Command "[System.TimeZoneInfo]::ConvertTimeBySystemTimeZoneId([DateTime]::UtcNow, 'China Standard Time').ToString('yyyyMMddHHmm')"
                OUTPUT_VARIABLE BEIJING_TIMESTAMP
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        else()
            # Linux/macOS 系统使用 date 命令
            execute_process(
                COMMAND date -u "+%Y%m%d%H%M" -d "+8 hours"
                OUTPUT_VARIABLE BEIJING_TIMESTAMP
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
        string(SUBSTRING "${BEIJING_TIMESTAMP}" 4 4 VERSION_PATCH)
        string(SUBSTRING "${BEIJING_TIMESTAMP}" 8 4 VERSION_TWEAK)
    endif()

    set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}" PARENT_SCOPE)
endfunction()