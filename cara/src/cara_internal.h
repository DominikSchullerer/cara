#ifndef CARA_INTERNAL_H_INCLUDED
#define CARA_INTERNAL_H_INCLUDED

#include <stdbool.h>
#include "cara/cara_config.h"
#include "platform/include/cara_platform.h"

typedef struct cara_timer_t cara_timer_t;
typedef struct cara_timer_channel_t cara_timer_channel_t;

struct cara_timer_channel_t {
	double dt;
	double accumulator;
};


struct cara_timer_t {
	double prev_time;
	double cur_time;
	double dt;

	cara_timer_channel_t update;
	cara_timer_channel_t render;
};

struct cara_t {
	cara_config_t  config;
	cara_timer_t timer;

	bool running;
	bool quit_requested;

	cara_platform_t *platform;
};


#endif // !CARA_INTERNAL_H_INCLUDED
