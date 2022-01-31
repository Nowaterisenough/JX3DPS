function(VERSION)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/modules/Common/Version.h" FILE_TEXT)

    string(REGEX MATCH "VERSION_MAJOR[ ]+([0-9]+)" _ "${FILE_TEXT}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()
    set(VERSION_MAJOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "VERSION_MINOR[ ]+([A-Z_A-Z]+)" _ "${FILE_TEXT}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()
    set(VERSION_JX3 ${CMAKE_MATCH_1})

    string(REGEX MATCH "${VERSION_JX3}[ ]+([0-9]+)" _ "${FILE_TEXT}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()
    set(VERSION_MINOR ${CMAKE_MATCH_1})
    
    find_package(Git QUIET)
    execute_process(COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE  GIT_REPO_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(CUSTOM FALSE)
    if(RET EQUAL "0")
        string(REGEX MATCH "Your branch is up to date with '.*/(.*)'.[\r|\n]+nothing to commit, working tree clean" _ "${GIT_REPO_STATUS}")
        if(NOT CMAKE_MATCH_COUNT EQUAL 1)
            set(CUSTOM TRUE)
        else()
            set(VERSION_BRANCH ${CMAKE_MATCH_1} PARENT_SCOPE)
        endif()
    else()
        set(CUSTOM TRUE)
    endif()

    execute_process(COMMAND ${GIT_EXECUTABLE} log ${VERSION_BRANCH} -1 --format=%cd --date=format:%y%m%d
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE  GIT_REPO_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(VERSION_PATCH ${GIT_REPO_DATE})

    execute_process(COMMAND ${GIT_EXECUTABLE} log ${VERSION_BRANCH} -1 --format=%cd --date=format:%H%M
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE  GIT_REPO_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RET
    )
    set(VERSION_TWEAK ${GIT_REPO_TIME})

    if(CUSTOM)
        set(VERSION_BRANCH "local" PARENT_SCOPE)
        string(TIMESTAMP VERSION_PATCH "%y%m%d")
        string(TIMESTAMP VERSION_TWEAK "%H%M")
    endif()
    
    set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}" PARENT_SCOPE)
endfunction()