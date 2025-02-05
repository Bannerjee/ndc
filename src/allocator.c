#include <ndc/allocator.h>
#include <ndc/error.h>
#include <stdio.h>
#include <stdlib.h>

size_t total_allocated = 0;

void* ndc_malloc(size_t requested_size)
{
	size_t real_size = requested_size + sizeof(size_t);
	void* ptr = malloc(real_size);
	if(!ptr) return NULL;
	*(size_t*)ptr = requested_size;

	char buff[50];
	buff[49] ='\0';
	sprintf(buff,"Allocated bytes:%llu",real_size);

	//NDC_INFO(buff);
	total_allocated += real_size;

	return (void*)((char*)ptr + sizeof(size_t));
}
void ndc_free(void* ptr)
{
	NDC_ASSERT(ptr);

	void* real = (char*)ptr - sizeof(size_t);
	size_t size = *(size_t*)real + sizeof(size_t);
    total_allocated -= size;
    char buff[50];
	buff[49] ='\0';
	sprintf(buff,"Freed bytes:%llu",size);
	//NDC_INFO(buff);
    free(real);
    ptr = NULL;
}
void* ndc_calloc(size_t count,size_t size)
{
	size_t total = count * size;
	return ndc_malloc(total);
}
void* ndc_realloc(void* ptr,size_t size)
{
	if (!ptr) return ndc_malloc(size);

    void* real_ptr = (char*)ptr - sizeof(size_t);
    size_t old_size = *(size_t*)real_ptr;

    void* new_ptr = realloc(real_ptr, size + sizeof(size_t));
    if (!new_ptr) return NULL;

    *(size_t*)new_ptr = size;
    total_allocated += (size - old_size);

    return (void*)((char*)new_ptr + sizeof(size_t));
}
size_t ndc_get_total_allocated()
{
	return total_allocated;
}