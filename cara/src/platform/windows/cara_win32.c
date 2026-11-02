#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <stdbool.h>

#include "cara_platform/cara_platform.h"
#include "cara_win32_internal.h"
#include "cara_win32_window.h"
#include "cara_core/cara_core.h"

static cara_status_t init_timer(cara_platform_t *platform);


cara_status_t cara_platform_create(
	cara_t *cara,
	const cara_window_config_t *window_config,
	cara_platform_t **p_platform
)
{
	*p_platform = NULL;

	cara_platform_t *platform = calloc(1, sizeof(*platform));
	if (!platform) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	cara_status_t status = CARA_STATUS_OK;

	status = init_timer(platform);
	if (status != CARA_STATUS_OK) {
		cara_platform_destroy(platform);
		return status;
	}

	platform->wait_timer = CreateWaitableTimerExW(
		NULL,
		NULL,
		CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
		TIMER_MODIFY_STATE | SYNCHRONIZE
	);
	if (!platform->wait_timer) {
		cara_platform_destroy(platform);
		return CARA_STATUS_PLATFORM_ERROR;
	}

	status = cara_window_create(window_config, platform, cara);
	if (status != CARA_STATUS_OK) {
		cara_platform_destroy(platform);
		return status;
	}

	platform->cara = cara;

	*p_platform = platform;

	return status;
}

void cara_platform_destroy(cara_platform_t *platform)
{
	if (!platform) {
		return;
	}

	if (platform->window) {
		DestroyWindow(platform->window);
		platform->window = NULL;
	}

	if (platform->wait_timer) {
		CloseHandle(platform->wait_timer);
		platform->wait_timer = NULL;
	}

	free(platform);
}

cara_status_t cara_platform_get_time(
	cara_platform_t *platform,
	double *p_time
) {
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
	if (wait_time < 0.0) {
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

cara_status_t cara_platform_process_messages(
	cara_platform_t *platform
) {
	cara_core_t *core = platform->cara->core;
	MSG msg;

	while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			cara_core_request_quit(core);
			break;
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return CARA_STATUS_OK;
}
