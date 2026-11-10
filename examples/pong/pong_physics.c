#include <stdlib.h>


#include "pong_physics.h"
#include "pong_constants.h"
#include "pong_types.h"


static collision_t find_earliest_collision(pong_t *pong, double time);


void simulate_physics(pong_t *pong, double delta_time)
{
	double remaining_time = delta_time;

	while (remaining_time > 0.0) {
		collision_t collision = find_earliest_collision(pong, remaining_time);
	}
}


static collision_t find_earliest_collision(pong_t *pong, double time)
{
	return (collision_t) { .type = COLLISION_NONE, .time = time };
}