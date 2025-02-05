#include <ndc/io.h>
#include <ndc/allocator.h>


#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
ndc_mapped_file* ndc_map_file(const char* filename)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
    
    if(hFile == INVALID_HANDLE_VALUE)
    {
    	NDC_WARNING("ndc_map_file - CreateFile failed");
        return NULL;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if(dwFileSize == INVALID_FILE_SIZE)
    {
    	NDC_WARNING("ndc_map_file - GetFileSize failed");
        CloseHandle(hFile);
        return NULL;
    }

    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0,NULL);
    if(hMapping == NULL)
    { 
    	NDC_WARNING("ndc_map_file - CreateFileMapping failed");
        CloseHandle(hFile);
        return NULL;
    }

    uint8_t* dataPtr = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
    if(dataPtr == NULL)
    {
    	NDC_WARNING("ndc_map_file - MapViewOfFile failed");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return NULL;
    }

    ndc_mapped_file* file = ndc_malloc(sizeof(ndc_mapped_file)); 

    file->hfile = hFile;
    file->hmapping = hMapping;
    file->data = dataPtr;
    file->size = dwFileSize;

    return file;
}
void ndc_unmap_file(ndc_mapped_file* f)
{
	UnmapViewOfFile(f->data);
    CloseHandle(f->hmapping);
    CloseHandle(f->hfile);
    ndc_free(f);
    f = NULL;
}
#endif

uint8_t* ndc_read_file(const char* filename,size_t* s)
{
	FILE* f = fopen(filename,"rb");
	if(!f) return NULL;
	fseek(f,0,SEEK_END);
	size_t file_size = ftell(f);
	rewind(f);
	uint8_t* data = ndc_malloc(file_size);
	fread(data,sizeof(uint8_t),file_size,f);
	fclose(f);  
	if(s) *s = file_size;
	return data;
}
uint8_t* ndc_read_bmp(const char* filename,int32_t* w,int32_t* h,uint16_t* bpp,ndc_error_t* t)
{
	uint8_t* data = ndc_read_file(filename,0);

	if(*(uint16_t*)data != 0x4D42)
	{
		if(t) *t = NDC_INVALID_IMAGE_MAGIC;
		return NULL;
	}
	uint32_t offset = *(uint32_t*)(data + 10);

	int32_t width = (*(int32_t*)(data + 18));
	int32_t height = (*(int32_t*)(data + 22));
	int16_t bit_depth = (*(uint16_t*)(data + 28));
	if(w) *w = width;
	if(h) *h = height;
	if(bpp) *bpp = bit_depth;

    int32_t row_size = ((bit_depth * width + 31) / 32) * 4;
    int32_t data_size = row_size * (height < 0 ? -height : height);
    uint8_t* pixels = ndc_malloc(data_size);

    memcpy(pixels,data + offset,data_size);
    ndc_free(data);
   	if(t) *t = NDC_SUCCESS;
   	return pixels;
}