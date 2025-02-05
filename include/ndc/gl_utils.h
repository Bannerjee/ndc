#pragma once

#include <stdint.h>

char ndc_link_program(uint32_t program);
uint32_t ndc_compile_shader(const char* src,uint32_t type);
uint32_t ndc_compile_program(uint32_t count,...);