#include <ndc/audio.h>
#include <ndc/error.h>
#include <ndc/allocator.h>

#ifdef _WIN32
#define UNICODE
#include <windows.h>

HMODULE audio_lib = NULL;
typedef UINT (WINAPI* PFNGETNUMAUDIODEVICES)();
typedef MMRESULT (WINAPI* PFNGETOUTDEVCAPS)(UINT uDeviceID,LPWAVEOUTCAPS pwoc,UINT cbwoc);


PFNGETNUMAUDIODEVICES get_num_devices = NULL;
PFNGETOUTDEVCAPS get_device_caps = NULL;
void* get_proc_addr(HMODULE m,const char* f)
{
	void* func = GetProcAddress(m,f);
	if(!func)
	{
		char buffer[100] = "failed to get proc addr:";
		strcat(buffer,f);
		NDC_WARNING(buffer);
		return NULL;
	}
	return func;
}
void ndc_audio_init()
{
	audio_lib = LoadLibraryA("Winmm.dll");
	get_num_devices = (PFNGETNUMAUDIODEVICES)get_proc_addr(audio_lib,"waveOutGetNumDevs");
	get_device_caps = (PFNGETOUTDEVCAPS)get_proc_addr(audio_lib,"waveOutGetDevCapsW");
}

void ndc_create_audio_device(
	uint8_t device_idx,
	uint32_t sampler_rate,
	uint32_t num_channels, 
	uint32_t num_blocks,
	uint32_t num_block_samples)
{

}

wchar_t** ndc_audio_get_devices(unsigned int* num_devices)
{
	NDC_ASSERT("[NDC]Audio lib is not initialized" && audio_lib && get_num_devices && get_device_caps);

	unsigned int devices =  get_num_devices();
	
	wchar_t** names = ndc_malloc(devices * sizeof(wchar_t*));
	WAVEOUTCAPS caps;
	for (int n = 0; n < devices; ++n)
	{
		if (get_device_caps(n, &caps, sizeof(WAVEOUTCAPS)) == S_OK)
		{
			size_t name_len = wcslen(caps.szPname);
			names[n] = ndc_malloc((name_len + 1) * sizeof(wchar_t));
			wcsncpy(names[n],caps.szPname,name_len);
			names[n][name_len] = '\0';
		}
	}
	if(num_devices) *num_devices = devices;
	return names;
}

void ndc_audio_free_devices(wchar_t** devices,unsigned int num_devices)
{
	for(unsigned int i=0;i<num_devices;++i)
	{
		ndc_free(devices[i]);
	}
	ndc_free(devices);
}
void ndc_audio_terminate()
{
	FreeLibrary(audio_lib);
}
#endif