#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cara/cara.h"


#define FPS				60
#define UPS				60
#define X_RESOLUTION	800
#define Y_RESOLUTION	600
#define PIXEL_COUNT		(X_RESOLUTION * Y_RESOLUTION)
#define PONG_TITLE		"Pong"

#define GAME_AREA_WIDTH		800.0
#define GAME_AREA_HEIGHT	600.0

#define SCOREBOARD_WIDTH	800.0
#define SCOREBOARD_HEIGHT	100.0

#define BACKGROUND_COLOR	0x00006000


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
	paddle_t paddle;
	uint32_t score;
} player_t;


typedef enum {
	AREA_SCOREBOARD,
	AREA_GAME,
	AREA_COUNT
} area_idx_t;


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
	area_t areas[AREA_COUNT];
	viewport_t viewports[AREA_COUNT];
} layout_t;

typedef struct {
	layout_t layouts[AREA_COUNT];
	double scale_x;
	double scale_y;
} render_context_t;

typedef struct {
	cara_pixbuf_t framebuffer;
	render_context_t contexts[AREA_COUNT];
} renderer_t;

typedef struct {
	ball_t ball;
	player_t players[2];
	bool is_running;

	renderer_t renderer;

	cara_t *cara;
} pong_t;


//////////////////////////////////////////////////////////////////////////////
// Callback declarations
//////////////////////////////////////////////////////////////////////////////

static cara_status_t pong_init(cara_app_t *app);

static cara_status_t pong_update(
	cara_app_t *app, 
	double delta_time
);

static cara_status_t pong_render(
	cara_app_t *app, 
	const cara_pixbuf_t **p_out_buffer
);

static cara_status_t pong_event(
	cara_app_t *app, 
	cara_event_t event
);

static cara_status_t pong_shutdown(cara_app_t *app);

//////////////////////////////////////////////////////////////////////////////
// Render declarations
//////////////////////////////////////////////////////////////////////////////

static void pong_render_scoreboard(pong_t *pong);
static void pong_render_gameboard(pong_t *pong);

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main(void)
{
	pong_t pong = {0};

	cara_app_t pong_app = {
		.data = &pong,
		.init = pong_init,
		.update = pong_update,
		.render = pong_render,
		.event = pong_event,
		.shutdown = pong_shutdown,
	};

	cara_config_t config = {
		.fps = FPS,
		.ups = UPS,
		.window.height = Y_RESOLUTION,
		.window.width = X_RESOLUTION,
		.window.title = PONG_TITLE,
	};

	if (cara_create(&config, &pong.cara) != CARA_STATUS_OK) {
		return EXIT_FAILURE;
	}
	
	if (cara_run(pong.cara, &pong_app) != CARA_STATUS_OK) {
		return EXIT_FAILURE;
	}

	cara_destroy(pong.cara);

	return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// App callbacks
//////////////////////////////////////////////////////////////////////////////

static cara_status_t pong_init(cara_app_t *app)
{
	pong_t *pong = app->data;
	cara_pixbuf_t *framebuffer = &pong->renderer.framebuffer;

	framebuffer->pixels = calloc(
		X_RESOLUTION * Y_RESOLUTION, 
		sizeof(*framebuffer->pixels)
	);
	if (!framebuffer->pixels) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	framebuffer->height = Y_RESOLUTION;
	framebuffer->width = X_RESOLUTION;

	return CARA_STATUS_OK;
}

static cara_status_t pong_update(
	cara_app_t *app,
	double delta_time
)
{
	(void) app;
	(void) delta_time;
	return CARA_STATUS_OK;
}

static cara_status_t pong_render(
	cara_app_t *app,
	const cara_pixbuf_t **p_out_buffer
)
{
	pong_t *pong = app->data;
	cara_pixbuf_t *framebuffer = &pong->renderer.framebuffer;

	*p_out_buffer = framebuffer;

	for (uint32_t i = 0; i < PIXEL_COUNT; i++) {
		framebuffer->pixels[i] = BACKGROUND_COLOR;
	}

	pong_render_scoreboard(pong);
	pong_render_gameboard(pong);

	return CARA_STATUS_OK;
}

static cara_status_t pong_event(
	cara_app_t *app,
	cara_event_t event
)
{
	pong_t *pong = app->data;
	
	switch (event.type) {
		case CARA_CLOSE_REQUEST:
			cara_request_quit(pong->cara);
	}

	return CARA_STATUS_OK;
}

static cara_status_t pong_shutdown(cara_app_t *app)
{
	(void) app;
	return CARA_STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////////
// Render functions
//////////////////////////////////////////////////////////////////////////////

static void pong_render_scoreboard(pong_t *pong)
{
	(void) pong;
}

static void pong_render_gameboard(pong_t *pong)
{
	(void) pong;
}

