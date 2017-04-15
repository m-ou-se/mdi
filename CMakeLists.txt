cmake_minimum_required(VERSION 3.1)
project(mdi)

add_subdirectory(src)

option(ENABLE_TESTS "Enable building of tests" on)
if (ENABLE_TESTS)
	add_subdirectory(test)
endif()
