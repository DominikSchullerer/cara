#ifndef CARA_PLATFORM_INTERNAL_H_INCLUDED
#define CARA_PLATFORM_INTERNAL_H_INCLUDED

#include <stdint.h>
#include <windows.h>
#include "platform/include/cara_platform.h"

struct cara_platform_t {
	uint64_t frequency;
	uint64_t start_timer;
	HANDLE wait_timer;
	HWND window;
};

#endif // !CARA_PLATFORM_INTERNAL_H_INCLUDED