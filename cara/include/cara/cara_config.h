#ifndef CARA_CONFIG_H_INCLUDED
#define CARA_CONFIG_H_INCLUDED

#include <stdint.h>

typedef struct cara_config_t cara_config_t;

struct cara_config_t {
	// updates per second, 0 := unlimited -> not yet implemented
	uint32_t ups;
	// frames per second, 0 := unlimited -> not yet implemented
	uint32_t fps;
};


#endif // !CARA_CONFIG_H_INCLUDED
