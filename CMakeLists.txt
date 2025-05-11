cmake_minimum_required(VERSION 4.0)

project(8086-simulator LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
add_compile_options(
                    -Wall -Wextra -Werror -pedantic -Wundef -Wfloat-equal -Wshadow
                    -Wpointer-arith -Wcast-align -Wstrict-prototypes -shared
                    -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -fpic -Wl -g
                    -Wswitch-default -Wconversion -Wunreachable-code -fpie -pie
                    -fasynchronous-unwind-tables -fexceptions
                    -fstack-clash-protection -fstack-protector
                    -fstack-protector-strong -grecord-gcc-switches
                    -fcf-protection -pipe -Werror=format-security
                    -Werror=implicit-function-declaration
)

add_library(decoder STATIC src/decoder.c include/decoder.h)
target_include_directories(decoder PUBLIC include)

add_executable(main src/main.c)
target_link_libraries(main PUBLIC decoder)

add_executable(tests tests/main.c)
target_link_libraries(tests PUBLIC decoder)
