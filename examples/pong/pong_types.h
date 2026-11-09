#ifndef PONG_TYPES_INCLUDED
#define PONG_TYPES_INCLUDED

typedef enum
{
	PLAYER_TYPE_HUMAN,
	PLAYER_TYPE_AI,
	PLAYER_TYPE_COUNT
} player_type_t;

typedef enum {
	AREA_SCOREBOARD,
	AREA_GAME,
	AREA_COUNT
} area_idx_t;



typedef struct {
	double x;
	double y;
} vec2_t;

typedef struct {
	double x;
	double y;
	double width;
	double height;
} rect_t;

typedef struct {
	rect_t rect;
	vec2_t velocity;
} ball_t;

typedef struct {
	rect_t rect;
	double speed;
} paddle_t;

typedef struct {
	player_type_t type;
	paddle_t paddle;
	uint32_t score;
} player_t;

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
	ball_t ball;
	player_t players[2];
	bool is_running;

	renderer_t renderer;

	cara_t *cara;
} pong_t;

#endif // !PONG_TYPES_INCLUDED
