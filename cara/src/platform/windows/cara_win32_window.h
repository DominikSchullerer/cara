#ifndef CARA_WIN32_WINDOW_H_INCLUDED
#define CARA_WIN32_WINDOW_H_INCLUDED

#include "cara/cara_status.h"
#include "cara/cara_config.h"
#include "cara_internal.h"

typedef struct cara_t cara_t;

#include "platform/windows/cara_win32_internal.h"

cara_status_t cara_window_create(
	const cara_window_config_t *window_config,
	cara_platform_t *platform,
	cara_t *cara
);

#endif // !CARA_WIN32_WINDOW_H_INCLUDED
