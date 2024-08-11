function(print_table)
    # 获取参数列表长度
    list(LENGTH ARGV arg_list_length)

    # 确保参数列表长度为偶数
    if(arg_list_length GREATER 0)
        math(EXPR is_even "${arg_list_length} % 2")

        if(is_even EQUAL 0)
            # 创建变量存储最长键和值的长度
            set(max_key_length 0)
            set(max_value_length 0)

            # 遍历参数列表来计算最长键和值的长度
            set(i 0)

            while(i LESS arg_list_length)
                list(GET ARGV ${i} key)
                math(EXPR next_i "${i} + 1")
                list(GET ARGV ${next_i} value)

                string(LENGTH "${key}" key_length)
                string(LENGTH "${value}" value_length)

                if(key_length GREATER max_key_length)
                    set(max_key_length ${key_length})
                endif()

                if(value_length GREATER max_value_length)
                    set(max_value_length ${value_length})
                endif()

                math(EXPR i "${i} + 2")
            endwhile()

            math(EXPR BORDER_WIDTH 5)
            string(REPEAT " " ${BORDER_WIDTH} BORDER_WIDTH_SPACES)

            # 计算表格的宽度
            math(EXPR table_width "${max_key_length} + ${max_value_length} + ${BORDER_WIDTH} * 2 + 2")

            # 打印表格的上边框
            string(REPEAT "-" ${table_width} border_line)
            message("")
            message("")
            message("+${border_line}+")

            # 打印表格的空白行
            string(REPEAT " " ${table_width} empty_line)
            message("|${empty_line}|")

            # 打印每一行的键值对
            set(i 0)

            while(i LESS arg_list_length)
                list(GET ARGV ${i} key)
                math(EXPR next_i "${i} + 1")
                list(GET ARGV ${next_i} value)

                string(LENGTH "${key}" key_length)
                string(LENGTH "${value}" value_length)

                # 计算填充空格
                math(EXPR key_padding "${max_key_length} - ${key_length}")
                math(EXPR value_padding "(${max_value_length} - ${value_length}) / 2")

                string(REPEAT " " ${key_padding} key_padding_str)
                string(REPEAT " " ${value_padding} value_padding_str)

                string(LENGTH "${BORDER_WIDTH_SPACES}${key}${key_padding_str} : ${value_padding_str}${value}${value_padding_str}${BORDER_WIDTH_SPACES}" length)

                if(length GREATER table_width)
                    math(EXPR padding "${BORDER_WIDTH} - (${length} - ${table_width})")
                else()
                    math(EXPR padding "${BORDER_WIDTH} - (${table_width} - ${length})")
                endif()

                string(REPEAT " " ${padding} padding_str)

                # 打印内容行
                message("|${BORDER_WIDTH_SPACES}${key}${key_padding_str} : ${value_padding_str}${value}${value_padding_str}${padding_str}|")

                # 打印表格的空白行
                message("|${empty_line}|")

                math(EXPR i "${i} + 2")
            endwhile()

            # 打印表格的下边框
            message("+${border_line}+")
            message("")
            message("")
        else()
            message(FATAL_ERROR "Arguments must be in key-value pairs")
        endif()
    else()
        message(FATAL_ERROR "Arguments must be in key-value pairs")
    endif()
endfunction()

function(get_version_date)
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

    execute_process(COMMAND ${GIT_EXECUTABLE} log ${REMOTE_BRANCH} -1 --format=%cd --date=format:%Y-%m-%d
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REPO_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(VERSION_DATE ${GIT_REPO_DATE} PARENT_SCOPE)

    if(CUSTOM)
        string(TIMESTAMP VERSION_DATE "%y-%m-%d")
    endif()
endfunction(get_version_date)
