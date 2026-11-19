function(VERSION)
    file(READ "${CMAKE_SOURCE_DIR}/src/Global/Version.h" FILE_TEXT)

    string(REGEX MATCH "VERSION_MAJOR[ ]+([0-9]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    set(VERSION_MAJOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "VERSION_MINOR[ ]+([a-zA-Z0-9_]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    string(REGEX MATCH "${CMAKE_MATCH_1}[ ]+([a-zA-Z0-9_]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    string(REGEX MATCH "${CMAKE_MATCH_1}[ ]+([0-9]+)" _ "${FILE_TEXT}")

    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from Version.h")
    endif()

    set(VERSION_MINOR ${CMAKE_MATCH_1})

    set(VERSION_BRANCH "local" PARENT_SCOPE)
    set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.231225.2335" PARENT_SCOPE)
endfunction()
