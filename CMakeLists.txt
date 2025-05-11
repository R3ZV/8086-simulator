cmake_minimum_required(VERSION 4.0)

project(8086-simulator LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-Wall -Wextra -pedantic -g)

add_library(decoder STATIC src/decoder.c include/decoder.h)
target_include_directories(decoder PUBLIC include)

add_executable(main src/main.c)
target_link_libraries(main PUBLIC decoder)

add_executable(tests tests/main.c)
target_link_libraries(tests PUBLIC decoder)
