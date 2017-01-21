list (APPEND chip8_sources
    main.c
    display.c
    chip8.c
    opcode.c
    )

set (chip8_sources "${chip8_sources}" PARENT_SCOPE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable (chip8 ${chip8_sources})

find_package(SDL)
find_package(SDL_gfx)
find_package(Threads)

target_link_libraries (chip8
    ${SDL_LIBRARY}
    ${SDLGFX_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    )
