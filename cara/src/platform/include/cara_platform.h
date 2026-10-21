#ifndef CARA_PLATFORM_H_INCLUDED
#define CARA_PLATFORM_H_INCLUDED

#include <stdint.h>
#include "cara/cara_status.h"

typedef struct cara_platform_t cara_platform_t;

cara_status_t cara_platform_create(cara_platform_t **p_platform);
void cara_platform_destroy(cara_platform_t *platform);

cara_status_t cara_platform_get_time(
    cara_platform_t *platform,
    double *p_time
);


#endif // !CARA_PLATFORM_H_INCLUDED