#ifndef CARA_INTERNAL_H_INCLUDED
#define CARA_INTERNAL_H_INCLUDED

#include <stdbool.h>
#include "cara/cara_config.h"

struct cara_t {
	cara_config_t  config;

	bool running;
	bool quit_requested;
};


#endif // !CARA_INTERNAL_H_INCLUDED
