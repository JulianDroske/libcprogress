# test/CMakeLists.txt

add_executable(test_cprogress test.c)

target_link_libraries(test_cprogress cprogress)

add_test(NAME CProgressTest COMMAND test_cprogress)
