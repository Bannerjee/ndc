#pragma once

#include <stddef.h>

void ndc_audio_init();
void ndc_audio_terminate();
wchar_t** ndc_audio_get_devices(unsigned int* num_devices);
void ndc_audio_free_devices(wchar_t** devices,unsigned int num_devices);