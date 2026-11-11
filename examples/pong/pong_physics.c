#include <stdlib.h>


#include "pong_physics.h"
#include "pong_constants.h"
#include "pong_types.h"


static collision_t find_earliest_collision(pong_t *pong, double time);
static void move_rect(rect_t *rect, vec2_t *velocity, double time);
static bool handle_collision(pong_t *pong, collision_t *collision);

static collision_t find_ball_score_collision(pong_t *pong, double time);
static collision_t find_ball_wall_collision(pong_t *pong, double time);


void simulate_physics(pong_t *pong, double delta_time)
{
	double remaining_time = delta_time;
	paddle_t *paddle_0 = &pong->players[0].paddle;
	paddle_t *paddle_1 = &pong->players[1].paddle;
	ball_t *ball = &pong->ball;

	while (remaining_time > 0.0) {
		collision_t collision = find_earliest_collision(pong, remaining_time);

		move_rect(&paddle_0->rect, &paddle_0->velocity, collision.time);
		move_rect(&paddle_1->rect, &paddle_1->velocity, collision.time);
		move_rect(&ball->rect, &ball->velocity, collision.time);

		remaining_time -= collision.time;

		if (!handle_collision(pong, &collision)) {
			break;
		}
	}
}


static collision_t find_earliest_collision(pong_t *pong, double time)
{
	collision_t earliest = {
		.type = COLLISION_NONE,
		.time = time
	};

	collision_t candidates[] = {
		find_ball_wall_collision(pong, time),
		// find_ball_paddle_collision(pong, 0, time),
		// find_ball_paddle_collision(pong, 1, time),
		// find_paddle_wall_collision(pong, 0, time),
		// find_paddle_wall_collision(pong, 1, time),
		find_ball_score_collision(pong, time)
	};

	for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
		if (candidates[i].time < earliest.time) {
			earliest = candidates[i];
		}
	}

	return earliest;
}

static void move_rect(rect_t *rect, vec2_t *velocity, double time)
{
	rect->upper_left.x += velocity->x * time;
	rect->upper_left.y += velocity->y * time;
}

static bool handle_collision(pong_t *pong, collision_t *collision)
{
	switch (collision->type) {
		case COLLISION_BALL_SCORE:
			// ToDo scoring logic + reset game etc.
			pong->ball.velocity.x *= -1;
			pong->players[collision->index].score += 1;
			break;
		case COLLISION_BALL_WALL:
			pong->ball.velocity.y *= -1;
			break;
	}

	return true;
}


static collision_t find_ball_score_collision(pong_t *pong, double time)
{
	collision_t collision = {
		.type = COLLISION_NONE,
		.time = time
	};

	double ball_x = pong->ball.rect.upper_left.x;
	double velocity_x = pong->ball.velocity.x;
	double ball_width = pong->ball.rect.width;

	if (velocity_x == 0.0) {
		return collision;
	}

	bool moving_right = velocity_x > 0.0;

	double ball_edge = moving_right
		? ball_x + ball_width
		: ball_x;

	double boundary = moving_right
		? GAME_AREA_WIDTH
		: 0.0;

	double collision_time = (boundary - ball_edge) / velocity_x;

	if (collision_time < 0.0 || collision_time > time) {
		return collision;
	}

	collision.type = COLLISION_BALL_SCORE;
	collision.time = collision_time;
	collision.normal_vect = (vec2_t) {
		.x = moving_right ? -1.0 : 1.0,
		.y = 0.0
	};
	collision.index = moving_right ? 0 : 1;

	return collision;
}


static collision_t find_ball_wall_collision(pong_t *pong, double time)
{
	collision_t collision = {
		.type = COLLISION_NONE,
		.time = time
	};

	double ball_y = pong->ball.rect.upper_left.y;
	double velocity_y = pong->ball.velocity.y;
	double ball_height = pong->ball.rect.width;

	if (velocity_y == 0.0) {
		return collision;
	}

	bool moving_down = velocity_y > 0.0;

	double ball_edge = moving_down
		? ball_y + ball_height
		: ball_y;

	double boundary = moving_down
		? GAME_AREA_HEIGHT
		: 0.0;

	double collision_time = (boundary - ball_edge) / velocity_y;

	if (collision_time < 0.0 || collision_time > time) {
		return collision;
	}

	collision.type = COLLISION_BALL_WALL;
	collision.time = collision_time;
	collision.normal_vect = (vec2_t) {
		.y = 0.0,
		.x = moving_down ? -1.0 : 1.0
	};

	return collision;
}

