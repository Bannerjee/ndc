cmake_minimum_required(VERSION 3.20)

project(ndc LANGUAGES C)
set(CMAKE_C_STANDARD 17)

find_package(Python COMPONENTS Interpreter REQUIRED)

set(NDC_DEBUG ON)
set(NDC_BUILD_EXAMPLE ON)

add_custom_target(NDC_GENERATED_OUTPUT DEPENDS include/ndc/gl.h)
if(NOT EXISTS "${CMAKE_HOME_DIRECTORY}/src/gl.c" OR NOT EXISTS "${CMAKE_HOME_DIRECTORY}/include/ndc/gl.h")
	execute_process(
		COMMAND ${Python_EXECUTABLE} scripts/gl_parser.py include/ndc/gl.h src/gl.c
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





