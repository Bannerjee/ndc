#pragma once
#include <ndc/error.h>

#include <stdint.h>

typedef struct 
{
	uint8_t* data;
	size_t size;
#ifdef _WIN32
	void* hmapping;
	void* hfile;
#endif
} ndc_mapped_file;

ndc_mapped_file* ndc_map_file(const char* filename);
void ndc_unmap_file(ndc_mapped_file* f);

uint8_t* ndc_read_file(const char* filename,size_t* s);
uint8_t* ndc_read_bmp(const char* filename,int32_t* w,int32_t* h,uint16_t* bpp,ndc_error_t* t);  