cmake_minimum_required(VERSION 3.5)
project(allocator-example VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -fsanitize=address")
set(C_STANDARD C11)

add_executable(allocator-example main.c)