cmake_minimum_required(VERSION 3.1)
project(mdi)

add_subdirectory(src)

if (CMAKE_COMPILER_IS_GNUCC)
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(ENABLE_TESTS "Enable building of tests" on)
if (ENABLE_TESTS)
	add_subdirectory(test)
endif()
