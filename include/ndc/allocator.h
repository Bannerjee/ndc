#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void* ndc_malloc(size_t s);
void* ndc_realloc(void* ptr,size_t size);
void* ndc_calloc(size_t count,size_t size);
// attempts to pass memory allocated not by ndc_*alloc result in ub
void ndc_free(void* mem);

size_t ndc_get_total_allocated();

#ifdef __cplusplus
}
#endif
