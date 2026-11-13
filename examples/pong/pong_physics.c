#include <stdlib.h>
#include <math.h>


#include "pong_physics.h"
#include "pong_constants.h"
#include "pong_types.h"


static collision_t find_earliest_collision(pong_t *pong, double time);
static void move_rect(rect_t *rect, vec2_t *velocity, double time);
static bool handle_collision(pong_t *pong, collision_t *collision);

static collision_t find_ball_score_collision(pong_t *pong, double time);
static collision_t find_ball_wall_collision(pong_t *pong, double time);
static collision_t find_ball_paddle_collision(pong_t *pong, uint32_t index, double time);
static collision_t find_paddle_wall_collision(pong_t *pong, uint32_t index, double time);


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
		find_ball_paddle_collision(pong, 0, time),
		find_ball_paddle_collision(pong, 1, time),
		find_paddle_wall_collision(pong, 0, time),
		find_paddle_wall_collision(pong, 1, time),
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
			pong->ball.rect.upper_left.x = BALL_START_X;
			pong->ball.rect.upper_left.y = BALL_START_Y;
			pong->ball.velocity.x = 0.0;
			pong->ball.velocity.y = 0.0;
			pong->is_running = false;
			break;
		case COLLISION_BALL_WALL:
			pong->ball.velocity.y *= -1;
			break;
		case COLLISION_BALL_PADDLE:
		{
			vec2_t *velocity = &pong->ball.velocity;
			vec2_t normal = collision->normal_vect;

			double dot = velocity->x * normal.x
				+ velocity->y * normal.y;

			velocity->x -= 2.0 * dot * normal.x;
			velocity->y -= 2.0 * dot * normal.y;
			break;
		}
		case COLLISION_PADDLE_WALL:
		{

			vec2_t *velocity = &pong->players[collision->index].paddle.velocity;
			vec2_t normal = collision->normal_vect;

			if (normal.x != 0.0) {
				velocity->x = 0.0;
			}
			if (normal.y != 0.0) {
				velocity->y = 0.0;
			}
			break;
		}
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

static collision_t find_ball_paddle_collision(pong_t* pong, uint32_t index, double time)
{
	collision_t collision = {
		.type = COLLISION_NONE,
		.time = time
	};

	ball_t *ball = &pong->ball;
	paddle_t *paddle = &pong->players[index].paddle;

	vec2_t relative_velocity = {
		.x = ball->velocity.x - paddle->velocity.x,
		.y = ball->velocity.y - paddle->velocity.y
	};

	double min_x = paddle->rect.upper_left.x - ball->rect.width;
	double max_x = paddle->rect.upper_left.x + paddle->rect.width;
	double min_y = paddle->rect.upper_left.y - ball->rect.height;
	double max_y = paddle->rect.upper_left.y + paddle->rect.height;

	double ball_x = ball->rect.upper_left.x;
	double ball_y = ball->rect.upper_left.y;

	double tx1, tx2, ty1, ty2;

	if (relative_velocity.x == 0.0) {
		if (ball_x < min_x || ball_x > max_x) {
			return collision;
		}

		tx1 = -INFINITY;
		tx2 = INFINITY;
	}
	else {
		tx1 = (min_x - ball_x) / relative_velocity.x;
		tx2 = (max_x - ball_x) / relative_velocity.x;
	}

	if (relative_velocity.y == 0) {
		if (ball_y < min_y || ball_y > max_y) {
			return collision;
		}

		ty1 = -INFINITY;
		ty2 = INFINITY;
	} 
	else {
		ty1 = (min_y - ball_y) / relative_velocity.y;
		ty2 = (max_y - ball_y) / relative_velocity.y;
	}

	double x_entry = fmin(tx1, tx2);
	double x_exit = fmax(tx1, tx2);
	double y_entry = fmin(ty1, ty2);
	double y_exit = fmax(ty1, ty2);

	double entry = fmax(x_entry, y_entry);
	double exit = fmin(x_exit, y_exit);

	if (entry > exit || exit < 0.0 || entry < 0.0 || entry > time) {
		return collision;
	}

	collision.type = COLLISION_BALL_PADDLE;
	collision.time = entry;
	collision.index = index;

	collision.normal_vect.x = 0.0;
	collision.normal_vect.y = 0.0;

	if (x_entry >= y_entry) {
		collision.normal_vect.x = relative_velocity.x > 0.0 ? -1.0 : 1.0;
	}
	if (y_entry >= x_entry) {
		collision.normal_vect.y = relative_velocity.y > 0.0 ? -1.0 : 1.0;
	}
	
	double length = sqrt(
		collision.normal_vect.x * collision.normal_vect.x +
		collision.normal_vect.y * collision.normal_vect.y
	);

	collision.normal_vect.x /= length;
	collision.normal_vect.y /= length;

	return collision;
}

static collision_t find_paddle_wall_collision(pong_t *pong, uint32_t index, double time)
{
	collision_t collision = {
		.type = COLLISION_NONE,
		.time = time
	};

	paddle_t *paddle = &pong->players[index].paddle;
	
	double collision_time_y = INFINITY;
	double collision_time_x = INFINITY;

	double velocity_y = paddle->velocity.y;
	if (velocity_y != 0.0) {
		double paddle_y = paddle->rect.upper_left.y;
		double paddle_height = paddle->rect.height;
		bool moving_down = velocity_y > 0.0;
		double paddle_edge = moving_down ? paddle_y + paddle_height : paddle_y;
		double boundary = moving_down ? GAME_AREA_HEIGHT : 0.0;
		collision_time_y = (boundary - paddle_edge) / velocity_y;
	}

	double velocity_x = paddle->velocity.x;
	if (velocity_x != 0.0) {
		double paddle_x = paddle->rect.upper_left.x;
		double paddle_width = paddle->rect.width;
		bool moving_right = velocity_x > 0.0;
		double paddle_edge = moving_right ? paddle_x + paddle_width : paddle_x;
		double boundary = moving_right ? GAME_AREA_WIDTH : 0.0;
		collision_time_x = (boundary - paddle_edge) / velocity_x;
	}

	if (collision_time_x < collision_time_y) {
		collision.normal_vect.x = velocity_x > 0.0 ? -1.0 : 1.0;
		collision.normal_vect.y = 0.0;
	}
	else if (collision_time_y < INFINITY) {
		collision.normal_vect.x = 0.0;
		collision.normal_vect.y = velocity_y > 0.0 ? -1.0 : 1.0;
	}
	double earliest_time = fmin(collision_time_x, collision_time_y);

	if (earliest_time >= 0.0 && earliest_time <= time) {
		collision.type = COLLISION_PADDLE_WALL;
		collision.time = earliest_time;
		collision.index = index;
	}

	return collision;
}

