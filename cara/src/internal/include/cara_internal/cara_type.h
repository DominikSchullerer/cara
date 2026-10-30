#ifndef CARA_INTERNAL_H_INCLUDED
#define CARA_INTERNAL_H_INCLUDED

#include <stdbool.h>

#include "platform/cara_platform_type.h"
#include "event/cara_event_manager_type.h"
#include "cara_core/cara_core_type.h"

typedef struct cara_timer_t cara_timer_t;
typedef struct cara_timer_channel_t cara_timer_channel_t;
typedef struct cara_t cara_t;

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
	// todo: move to core
	cara_timer_t timer;
	bool running;
	bool quit_requested;


	cara_core_t *core;
	cara_platform_t *platform;
	cara_event_manager_t *event_manager;
};


#endif // !CARA_INTERNAL_H_INCLUDED
