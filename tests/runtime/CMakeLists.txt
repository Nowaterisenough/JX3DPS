find_package(Threads REQUIRED)

add_executable(runtime_tests runtime_tests.cpp)
target_include_directories(runtime_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(runtime_tests PRIVATE cxx_std_26)
target_link_libraries(runtime_tests PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(runtime_tests PRIVATE /W4 /utf-8)
else()
    target_compile_options(runtime_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_test(NAME runtime_tests COMMAND runtime_tests)

add_executable(runtime_benchmark runtime_benchmark.cpp)
target_include_directories(runtime_benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(runtime_benchmark PRIVATE cxx_std_26)
target_link_libraries(runtime_benchmark PRIVATE Threads::Threads)

add_executable(tai_xu_tests tai_xu_tests.cpp)
target_include_directories(tai_xu_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(tai_xu_tests PRIVATE cxx_std_26)
target_link_libraries(tai_xu_tests PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(tai_xu_tests PRIVATE /W4 /utf-8)
else()
    target_compile_options(tai_xu_tests PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()
add_test(NAME tai_xu_tests COMMAND tai_xu_tests)

add_executable(tai_xu_runtime ../../apps/cli/tai_xu_runtime.cpp)
target_include_directories(tai_xu_runtime PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(tai_xu_runtime PRIVATE cxx_std_26)
target_link_libraries(tai_xu_runtime PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(tai_xu_runtime PRIVATE /W4 /utf-8)
else()
    target_compile_options(tai_xu_runtime PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_test(NAME tai_xu_cli COMMAND tai_xu_runtime --iterations 16 --workers 2 --talents dieren,wuyi,shenmai)
add_test(NAME tai_xu_cli_equipment COMMAND tai_xu_runtime --iterations 16 --workers 2 --equipment weapon_cw,set_attribute)
add_test(NAME tai_xu_cli_rejects_unmigrated COMMAND tai_xu_runtime --talents yunzhongjian)
set_tests_properties(tai_xu_cli_rejects_unmigrated PROPERTIES WILL_FAIL TRUE)
add_test(NAME tai_xu_cli_fields COMMAND tai_xu_runtime --iterations 16 --workers 2
    --macro "${CMAKE_CURRENT_SOURCE_DIR}/macros/tai_xu_fields.txt"
    --talents dieren,wuyi,shenmai,fuyin,guchang,qisheng,changsheng
    --recipes-shengtaiji prepare1,prepare2,prepare3,cooldown)
add_test(NAME tai_xu_cli_explosions COMMAND tai_xu_runtime --iterations 16 --workers 2
    --macro "${CMAKE_CURRENT_SOURCE_DIR}/macros/tai_xu_explosions.txt"
    --talents dieren,wuyi,shenmai,fengshi,wuyu,xingu,fuyin,guchang,qisheng,changsheng,xuanmen,lieyun,xuji
    --attribute overcome_base=20000 --attribute overcome_by_class=5000
    --recipes-renjian cooldown,dot,damage40,damage60)
add_test(NAME tai_xu_cli_rejects_unsplit_overcome COMMAND tai_xu_runtime --talents xuanmen)
set_tests_properties(tai_xu_cli_rejects_unsplit_overcome PROPERTIES WILL_FAIL TRUE)

add_executable(mo_wen_tests mo_wen_tests.cpp)
target_include_directories(mo_wen_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(mo_wen_tests PRIVATE cxx_std_26)
target_link_libraries(mo_wen_tests PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(mo_wen_tests PRIVATE /W4 /utf-8)
else()
    target_compile_options(mo_wen_tests PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()
add_test(NAME mo_wen_tests COMMAND mo_wen_tests)

add_executable(mo_wen_runtime ../../apps/cli/mo_wen_runtime.cpp)
target_include_directories(mo_wen_runtime PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_compile_features(mo_wen_runtime PRIVATE cxx_std_26)
target_link_libraries(mo_wen_runtime PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(mo_wen_runtime PRIVATE /W4 /utf-8)
else()
    target_compile_options(mo_wen_runtime PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_test(NAME mo_wen_cli COMMAND mo_wen_runtime --iterations 16 --workers 2 --seconds 10)
add_test(NAME tai_xu_cli_team_buffs COMMAND tai_xu_runtime --iterations 8 --workers 2 --seconds 60
    --attribute overcome_base=25000 --team-buffs ji_lei:0:96:1 --team-buffs hao_ling_san_jun:16:0:48)
add_test(NAME tai_xu_cli_rejects_odd_drum COMMAND tai_xu_runtime --team-buffs hao_ling_san_jun:0:0:3)
set_tests_properties(tai_xu_cli_rejects_odd_drum PROPERTIES WILL_FAIL TRUE)
add_test(NAME mo_wen_cli_debug_replay COMMAND "${CMAKE_COMMAND}"
    "-DCLI=$<TARGET_FILE:mo_wen_runtime>" "-DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/mo_wen_cli_test"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/mo_wen_cli_test.cmake")
