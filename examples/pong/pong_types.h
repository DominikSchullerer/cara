#ifndef PONG_TYPES_INCLUDED
#define PONG_TYPES_INCLUDED

#include <stdbool.h>
#include <cara/cara.h>

typedef enum {
	AREA_SCOREBOARD,
	AREA_GAME,
	AREA_COUNT
} area_idx_t;

typedef enum {
	COLLISION_NONE,
	COLLISION_BALL_WALL,
	COLLISION_BALL_PADDLE,
	COLLISION_BALL_SCORE,
	COLLISION_PADDLE_WALL
} collision_type_t;


typedef struct {
	double x;
	double y;
} vec2_t;

typedef struct {
	vec2_t upper_left;
	double width;
	double height;
} rect_t;

typedef struct {
	rect_t rect;
	vec2_t velocity;
} ball_t;

typedef struct {
	rect_t rect;
	vec2_t velocity;
} paddle_t;

typedef struct {
	paddle_t paddle;
	uint32_t score;
} player_t;

typedef struct {
	collision_type_t type;
	vec2_t normal_vect;
	double time;
	uint32_t index;
} collision_t;



typedef struct {
	double width;
	double height;
} area_t;

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
} viewport_t;

typedef struct {
	area_t area;
	viewport_t viewport;
	double scale_x;
	double scale_y;
} layout_t;

typedef struct {
	cara_pixbuf_t framebuffer;
	layout_t layouts[AREA_COUNT];
} renderer_t;


typedef struct {
	cara_key_t up;
	cara_key_t down;
} player_control_t;

typedef struct {
	player_control_t player_controls[2];
	cara_key_t exit;
} control_t;


typedef struct {
	ball_t ball;
	player_t players[2];
	bool is_running;

	control_t control;

	renderer_t renderer;

	cara_t *cara;
} pong_t;

#endif // !PONG_TYPES_INCLUDED
