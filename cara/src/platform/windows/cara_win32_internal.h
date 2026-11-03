#ifndef CARA_PLATFORM_INTERNAL_H_INCLUDED
#define CARA_PLATFORM_INTERNAL_H_INCLUDED

#include <stdint.h>
#include <windows.h>

#include "cara_platform/cara_platform_type.h"
#include "cara/cara_pixbuf.h"

#include "cara_internal/cara_type.h"

struct cara_platform_t {
	cara_t *cara;
	uint64_t frequency;
	uint64_t start_timer;
	HANDLE wait_timer;
	HWND window;
	const cara_pixbuf_t *pixbuf;
};

#endif // !CARA_PLATFORM_INTERNAL_H_INCLUDED