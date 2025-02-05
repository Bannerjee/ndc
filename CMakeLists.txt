cmake_minimum_required(VERSION 3.20)

project(ndc LANGUAGES C)
set(CMAKE_C_STANDARD 17)

find_package(Python COMPONENTS Interpreter REQUIRED)

set(NDC_DEBUG ON)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/src/gl.c" OR NOT EXISTS "${PROJECT_SOURCE_DIR}/include/ndc/gl.h")
	execute_process(
		COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/gl_parser.py ${PROJECT_SOURCE_DIR}/include/ndc/gl.h ${PROJECT_SOURCE_DIR}/src/gl.c
	)
endif()

add_library(NDC STATIC
	src/io.c
	src/window.c
	src/error.c
	src/gl.c
	src/allocator.c
	src/audio.c
	src/math.c
	src/gl_utils.c
	)


target_include_directories(NDC PUBLIC include)
if(NDC_DEBUG)
	target_compile_options(NDC PRIVATE -Wall -g -ggdb -O0 -DNDC_DEBUG)
else()
	target_compile_options(NDC PRIVATE -O3 -march=native)
endif()





