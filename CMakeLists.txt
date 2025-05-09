cmake_minimum_required(VERSION 3.15...4.0)

project(8086-simulator LANGUAGES C)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -pedantic -g)

add_executable(main src/main.c src/decoder.c)
