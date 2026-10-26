#ifndef CARA_INTERNAL_H_INCLUDED
#define CARA_INTERNAL_H_INCLUDED

#include <stdbool.h>
#include "cara/cara_config.h"
#include "platform/include/cara_platform.h"

struct cara_t {
	cara_config_t  config;
	cara_platform_t *platform;

	bool running;
	bool quit_requested;
};


#endif // !CARA_INTERNAL_H_INCLUDED
