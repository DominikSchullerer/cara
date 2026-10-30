#ifndef CARA_PLATFORM_H_INCLUDED
#define CARA_PLATFORM_H_INCLUDED

#include <stdbool.h>

#include "cara/cara_status.h"
#include "cara/cara_config.h"
#include "platform/cara_platform_type.h"
#include "cara_internal/cara_type.h"

cara_status_t cara_platform_create(
    cara_platform_t **p_platform, 
    const cara_window_config_t *window_config,
    cara_t* cara
);

void cara_platform_destroy(cara_platform_t *platform);

cara_status_t cara_platform_get_time(
    cara_platform_t *platform,
    double *p_time
);

cara_status_t cara_platform_wait(
    cara_platform_t *platform,
    double wait_time
);

cara_status_t cara_platform_process_messages(
    cara_platform_t *platform,
    bool *p_quit_requested
);


#endif // !CARA_PLATFORM_H_INCLUDED