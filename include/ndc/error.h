#pragma once

#include <assert.h>

typedef enum
{
	NDC_INFO,		// everything else
	NDC_WARNING,	// application may work under certain conditions/perf hit
	NDC_CRITICAL 	// application cannot function properly
} ndc_error_severity_t;

typedef enum 
{
	NDC_SUCCESS,

	// io
	NDC_FILE_OPENING_FAILURE,

	NDC_CONTEXT_CREATION_FAILURE,

	// asset related
	NDC_INVALID_BPP,
	NDC_INVALID_COLOR_TYPE,
	NDC_INVALID_IMAGE_MAGIC,
	NDC_INVALID_IMAGE_COMPRESSION,
	NDC_INVALID_COLOR_TYPE_BPP_COMBO
} ndc_error_t;

void ndc_report(const char* msg,ndc_error_severity_t severity);
const char* ndc_get_error_msg(ndc_error_t err);

#ifdef NDC_DEBUG
	#define NDC_ASSERT(msg) (assert(msg))
	#define NDC_INFO(msg) (ndc_report(msg,NDC_INFO))
	#define NDC_CRITICAL(msg) (ndc_report(msg,NDC_CRITICAL))
	#define NDC_WARNING(msg) (ndc_report(msg,NDC_WARNING))
#else
	#define NDC_ASSERT(msg)
	#define NDC_INFO(msg)
	#define NDC_CRITICAL(msg)
	#define NDC_WARNING(msg)
#endif


/*
#ifndef NDC_ASSERT
  #ifdef NDC_DEBUG
    #define FWOG_ASSERT(x) assert(x)
  #else
    #define FWOG_ASSERT(x) (void)(x)
  #endif
#endif*/