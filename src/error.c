#include <ndc/error.h>

#include <stdio.h>

void ndc_report(const char* msg,ndc_error_severity_t severity)
{
	switch(severity)
	{
		case NDC_INFO:		printf("[INFO]");			break;
		case NDC_WARNING:	printf("[WARNING]");		break;
		case NDC_CRITICAL:	printf("[CRITICAL]");		break;
	}
	printf("%s\n",msg);
}
const char* ndc_get_error_msg(ndc_error_t err)
{
	switch(err)
	{
		case NDC_SUCCESS: return "SUCCESS";
		case NDC_FILE_OPENING_FAILURE: return "File opening failure";

		case NDC_INVALID_BPP: return "Invalid bit depth value";
		case NDC_INVALID_COLOR_TYPE: return "Invalid color type";
		case NDC_INVALID_IMAGE_MAGIC: return "Invalid image magic";
		case NDC_INVALID_IMAGE_COMPRESSION: return "Invalid image compression";
		case NDC_INVALID_COLOR_TYPE_BPP_COMBO: return "Invalid color type + bit depth combo";
		default: return "UNKNOWN";
	};
}