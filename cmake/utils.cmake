function(print_bordered_info  project_name project_version version_branch)
    # 计算边框宽度
    math(EXPR BORDER_WIDTH 50)
    math(EXPR BORDER_WIDTH_LEFT 10)
    string(REPEAT " " ${BORDER_WIDTH_LEFT} BORDER_WIDTH_SPACES)
    math(EXPR BORDER_WIDTH_MID 5)
    string(REPEAT " " ${BORDER_WIDTH_MID} BORDER_WIDTH_MID_SPACES)

    # 输出顶部边框
    string(REPEAT "-" ${BORDER_WIDTH} BORDER_LINE)
    string(REPEAT " " ${BORDER_WIDTH} BORDER_SPACE)
    message(STATUS "+${BORDER_LINE}+")
    message(STATUS "|${BORDER_SPACE}|")

    # 输出内容行
    # 计算最长字符串的长度
    string(LENGTH "${BORDER_WIDTH_SPACES}PROJECT:${BORDER_WIDTH_MID_SPACES}${project_name}" NAME_LENGTH)
    string(LENGTH "${BORDER_WIDTH_SPACES}VERSION:${BORDER_WIDTH_MID_SPACES}${project_version}" VERSION_LENGTH)
    string(LENGTH "${BORDER_WIDTH_SPACES}BRANCH: ${BORDER_WIDTH_MID_SPACES}${version_branch}" BRANCH_LENGTH)

    # 计算每个字段的最小宽度
    math(EXPR NAME_WIDTH "${BORDER_WIDTH} - 3 - ${NAME_LENGTH}")
    math(EXPR VERSION_WIDTH "${BORDER_WIDTH} - 3 - ${VERSION_LENGTH}")
    math(EXPR BRANCH_WIDTH "${BORDER_WIDTH} - 3 - ${BRANCH_LENGTH}")

    # 使用 string(REPEAT ...) 来生成补位的空格
    string(REPEAT " " ${NAME_WIDTH} NAME_WIDTH_SPACES)
    string(REPEAT " " ${VERSION_WIDTH} VERSION_WIDTH_SPACES)
    string(REPEAT " " ${BRANCH_WIDTH} BRANCH_WIDTH_SPACES)

    # 输出每个字段，并在字段值后面添加适当数量的空格以对齐
    message(STATUS "| ${BORDER_WIDTH_SPACES}PROJECT: ${BORDER_WIDTH_MID_SPACES}${project_name}${NAME_WIDTH_SPACES} |")
    message(STATUS "| ${BORDER_WIDTH_SPACES}VERSION: ${BORDER_WIDTH_MID_SPACES}${project_version}${VERSION_WIDTH_SPACES} |")
    message(STATUS "| ${BORDER_WIDTH_SPACES}BRANCH:  ${BORDER_WIDTH_MID_SPACES}${version_branch}${BRANCH_WIDTH_SPACES} |")

    # 输出底部边框
    message(STATUS "|${BORDER_SPACE}|")
    message(STATUS "+${BORDER_LINE}+")
endfunction()