list (APPEND test_chip8_sources
    test_main.c
    test_chip8.c
    test_opcode.c
    ../src/chip8.c 
    #../src/memory.c 
    ../src/opcode.c
    )

set (test_chip8_sources "${test_chip8_sources}" PARENT_SCOPE)

find_package (Threads)
find_package (Check)

add_executable (test_chip8 ${test_chip8_sources} )
add_definitions (-fprofile-arcs -ftest-coverage)
target_link_libraries (test_chip8
    -fprofile-arcs
    ${CHECK_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )
