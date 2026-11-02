#ifndef CARA_INTERNAL_H_INCLUDED
#define CARA_INTERNAL_H_INCLUDED

#include <stdbool.h>

#include "cara_platform/cara_platform_type.h"
#include "cara_event/cara_event_manager_type.h"
#include "cara_core/cara_core_type.h"
#include "cara/cara_config.h"

typedef struct cara_t cara_t;

/**
 * @brief Internal runtime state
 */
struct cara_t {
	cara_config_t config;
	cara_core_t *core;
	cara_platform_t *platform;
	cara_event_manager_t *event_manager;
};


#endif // !CARA_INTERNAL_H_INCLUDED
