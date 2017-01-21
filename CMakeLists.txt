cmake_minimum_required (VERSION 2.6)
project (chip8 C)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")

enable_testing()

add_subdirectory (src)
add_subdirectory (test)
add_test(NAME test_chip8 COMMAND test_chip8)
