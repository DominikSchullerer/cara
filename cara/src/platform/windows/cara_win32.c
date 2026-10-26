#include <stdlib.h>

#include <windows.h>


#include "platform/include/cara_platform.h"
#include "platform/windows/cara_win32_internal.h"


static cara_status_t init_timer(cara_platform_t *platform);


cara_status_t cara_platform_create(cara_platform_t **p_platform)
{
	if (!p_platform) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}
	*p_platform = NULL;

	cara_platform_t *platform = calloc(1, sizeof(*platform));
	if (!platform) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	cara_status_t status = CARA_STATUS_OK;

	status = init_timer(platform);
	if (status != CARA_STATUS_OK) {
		free(platform);
		return status;
	}

	platform->wait_timer = CreateWaitableTimerExW(
		NULL,
		NULL,
		CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
		TIMER_MODIFY_STATE | SYNCHRONIZE
	);
	if (!platform->wait_timer) {
		free(platform);
		return CARA_STATUS_PLATFORM_ERROR;
	}

	*p_platform = platform;

	return status;
}

void cara_platform_destroy(cara_platform_t *platform)
{
	if (!platform) {
		return;
	}

	CloseHandle(platform->wait_timer);
	free(platform);
}

cara_status_t cara_platform_get_time(
	cara_platform_t *platform,
	double *p_time
) {
	if (!platform || !p_time) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	LARGE_INTEGER counter;
	if (!QueryPerformanceCounter(&counter) ||
		counter.QuadPart < 0) {
		return CARA_STATUS_PLATFORM_ERROR;
	}
	uint64_t current_timer = (uint64_t) counter.QuadPart;

	if (current_timer < platform->start_timer ||
		platform->frequency == 0) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	uint64_t elapsed_ticks =
		current_timer - platform->start_timer;

	*p_time = (double) elapsed_ticks /
		(double) platform->frequency;

	return CARA_STATUS_OK;
}


static cara_status_t init_timer(cara_platform_t *platform) 
{
	if (!platform) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	LARGE_INTEGER frequency;
	LARGE_INTEGER counter;

	if (!QueryPerformanceFrequency(&frequency) ||
		!QueryPerformanceCounter(&counter) ||
		frequency.QuadPart <= 0 ||
		counter.QuadPart < 0) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	platform->frequency = (uint64_t) frequency.QuadPart;
	platform->start_timer = (uint64_t) counter.QuadPart;

	return CARA_STATUS_OK;
}

cara_status_t cara_platform_wait(
	cara_platform_t *platform,
	double wait_time
) {
	if (!platform || wait_time < 0.0) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (wait_time == 0.0) {
		return CARA_STATUS_OK;
	}

	LARGE_INTEGER due_time = {0};
	due_time.QuadPart = -(LONGLONG) (wait_time * 10000000.0);

	if (!SetWaitableTimer(
		platform->wait_timer,
		&due_time,
		0,
		NULL,
		NULL,
		FALSE)) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	DWORD result = WaitForSingleObject(platform->wait_timer, INFINITE);
	
	if (result != WAIT_OBJECT_0) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	return CARA_STATUS_OK;
}
