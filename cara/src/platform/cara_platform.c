#include "platform/include/cara_platform.h"
#include "platform/cara_platform_internal.h"


cara_status_t cara_platform_create(cara_platform_t **p_platform)
{
	(void) p_platform;
	return CARA_STATUS_OK;
}

uint64_t cara_platform_get_time(cara_platform_t *platform)
{
	(void) platform;
	return 0;
}

void cara_platform_destroy(cara_platform_t *platform)
{
	(void) platform;
	;
}
