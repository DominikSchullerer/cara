#ifndef CARA_CONFIG_H_INCLUDED
#define CARA_CONFIG_H_INCLUDED

#include <stdint.h>

typedef struct cara_config_t cara_config_t;
typedef struct cara_window_config_t cara_window_config_t;

struct cara_window_config_t {
	char *title;
	uint32_t width;
	uint32_t height;
};

struct cara_config_t {
	// updates per second, 0 := unlimited
	uint32_t ups;
	// frames per second, 0 := unlimited
	uint32_t fps;

	cara_window_config_t window;
};


#endif // !CARA_CONFIG_H_INCLUDED
