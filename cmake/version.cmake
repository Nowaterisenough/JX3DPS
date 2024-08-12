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

        # 获取 UTC 时间戳
        string(TIMESTAMP UTC_TIMESTAMP "%Y%m%d%H%M" UTC)

        # 输出 UTC 时间戳以进行调试
        message(STATUS "Generated UTC timestamp: ${UTC_TIMESTAMP}")

        if(UTC_TIMESTAMP)
            # 提取年、月、日、小时、分钟
            string(SUBSTRING "${UTC_TIMESTAMP}" 0 4 YEAR)
            string(SUBSTRING "${UTC_TIMESTAMP}" 4 2 MONTH)
            string(SUBSTRING "${UTC_TIMESTAMP}" 6 2 DAY)
            string(SUBSTRING "${UTC_TIMESTAMP}" 8 2 HOUR)
            string(SUBSTRING "${UTC_TIMESTAMP}" 10 2 MINUTE)

            # 将小时转换为整数并加 8
            math(EXPR HOUR "${HOUR} + 8")

            # 处理日期变更
            if(HOUR GREATER_EQUAL 24)
                math(EXPR HOUR "${HOUR} - 24")
                math(EXPR DAY "${DAY} + 1")

                # 这里可以添加更复杂的月份和年份处理逻辑，如果需要的话
            endif()

            # 格式化时间
            if(HOUR LESS 10)
                set(HOUR "0${HOUR}")
            endif()

            if(DAY LESS 10)
                set(DAY "0${DAY}")
            endif()

            # 构建北京时间戳
            set(BEIJING_TIMESTAMP "${YEAR}${MONTH}${DAY}${HOUR}${MINUTE}")
            message(STATUS "Converted Beijing timestamp: ${BEIJING_TIMESTAMP}")

            string(SUBSTRING "${BEIJING_TIMESTAMP}" 4 4 VERSION_PATCH)
            string(SUBSTRING "${BEIJING_TIMESTAMP}" 8 4 VERSION_TWEAK)
        else()
            message(WARNING "Failed to generate timestamp, using default values")
            set(VERSION_PATCH "0000")
            set(VERSION_TWEAK "0000")
        endif()
    endif()

    set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}" PARENT_SCOPE)
endfunction()