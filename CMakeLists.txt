cmake_minimum_required(VERSION 3.27.6)

project(aoc VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(
    -g3
    -Werror
    -Wall -Wextra -Wpedantic
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wmissing-declarations
    -Wshadow
    -Wunused
    -Wvla
    -Wwrite-strings
    $<$<COMPILE_LANGUAGE:CXX>:-Wzero-as-null-pointer-constant>
    $<$<COMPILE_LANGUAGE:C>:-Wmissing-prototypes>
    $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>
    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
    )

add_subdirectory(2015)
