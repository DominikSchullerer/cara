#ifndef CARA_PLATFORM_INTERNAL_H_INCLUDED
#define CARA_PLATFORM_INTERNAL_H_INCLUDED

#include <stdint.h>

struct cara_platform_t {
	uint64_t frequency;
	uint64_t start_timer;
};



#endif // !CARA_PLATFORM_INTERNAL_H_INCLUDED