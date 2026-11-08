#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "cara/cara.h"


#define FPS				60
#define UPS				60
#define X_RESOLUTION	800
#define Y_RESOLUTION	600
#define PIXEL_COUNT		(X_RESOLUTION * Y_RESOLUTION)
#define PONG_TITLE		"Pong"

#define GAME_AREA_WIDTH		1600.0
#define GAME_AREA_HEIGHT	1000.0

#define PADDLE_WIDTH		20.0
#define PADDLE_HEIGHT		100.0
#define PADDLE_OFFSET		50.0
#define PADDLE_START_Y		400.0
#define PADDLE_SPEED		400.0

#define BALL_WIDTH			20.0
#define BALL_HEIGHT		    20.0
#define BALL_START_X		790.0
#define BALL_START_Y		490.0
#define BALL_SPEED			400.0

#define SCOREBOARD_WIDTH	1600.0
#define SCOREBOARD_HEIGHT	200.0

#define BACKGROUDN_COLOR_SCOREBOARD 0x00000060
#define BACKGROUND_COLOR_GAME	    0x00006000
#define PADDLE_COLOR		        0x00FFFFFF
#define BALL_COLOR			        0x00FF00FF


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

static void pong_render_rect(
    renderer_t *renderer,
    area_idx_t area_idx,
    const rect_t *rect,
    uint32_t color
);

static void pong_render_fill_area(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t color
);

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
    renderer_t *renderer = &pong->renderer;
	cara_pixbuf_t *framebuffer = &renderer->framebuffer;

	framebuffer->pixels = calloc(PIXEL_COUNT, sizeof(*framebuffer->pixels));
	if (!framebuffer->pixels) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	framebuffer->height = Y_RESOLUTION;
	framebuffer->width = X_RESOLUTION;

    layout_t *scoreboard_layout = &renderer->layouts[AREA_SCOREBOARD];
    scoreboard_layout->area.width = SCOREBOARD_WIDTH;
    scoreboard_layout->area.height = SCOREBOARD_HEIGHT;
    scoreboard_layout->viewport.x = 0;
    scoreboard_layout->viewport.y = 0;
    scoreboard_layout->viewport.width = framebuffer->width;
    scoreboard_layout->viewport.height = 100;
    scoreboard_layout->scale_x = (double)scoreboard_layout->viewport.width / SCOREBOARD_WIDTH;
    scoreboard_layout->scale_y = (double)scoreboard_layout->viewport.height / SCOREBOARD_HEIGHT;

    layout_t *game_layout = &renderer->layouts[AREA_GAME];
    game_layout->area.width = GAME_AREA_WIDTH;
    game_layout->area.height = GAME_AREA_HEIGHT;
    game_layout->viewport.x = 0;
    game_layout->viewport.y = 100;
    game_layout->viewport.width = framebuffer->width;
    game_layout->viewport.height = 500;
    game_layout->scale_x = (double)game_layout->viewport.width / GAME_AREA_WIDTH;
    game_layout->scale_y = (double)game_layout->viewport.height / GAME_AREA_HEIGHT;

    pong->players[0].paddle.rect.x = PADDLE_OFFSET;
    pong->players[0].paddle.rect.y = PADDLE_START_Y;
    pong->players[0].paddle.rect.width = PADDLE_WIDTH;
    pong->players[0].paddle.rect.height = PADDLE_HEIGHT;
    pong->players[0].type = PLAYER_TYPE_HUMAN;

    pong->players[1].paddle.rect.x = GAME_AREA_WIDTH - PADDLE_OFFSET;
    pong->players[1].paddle.rect.y = PADDLE_START_Y;
    pong->players[1].paddle.rect.width = PADDLE_WIDTH;
    pong->players[1].paddle.rect.height = PADDLE_HEIGHT;
    pong->players[1].type = PLAYER_TYPE_HUMAN;

    pong->ball.rect.x = BALL_START_X;
    pong->ball.rect.y = BALL_START_Y;
    pong->ball.rect.width = BALL_WIDTH;
    pong->ball.rect.height = BALL_HEIGHT;

	return CARA_STATUS_OK;
}

static cara_status_t pong_update(
	cara_app_t *app,
	double delta_time
)
{
	(void) delta_time;
    pong_t *pong = app->data;

    if (pong->players[0].type == PLAYER_TYPE_HUMAN) {
        if (cara_key_down(pong->cara, KEY_W)) {
            pong->players[0].paddle.rect.y -= PADDLE_SPEED * delta_time;
        }
        if (cara_key_down(pong->cara, KEY_S)) {
            pong->players[0].paddle.rect.y += PADDLE_SPEED * delta_time;
        }
        if (pong->players[0].paddle.rect.y < 0) {
            pong->players[0].paddle.rect.y = 0;
        }
        if (pong->players[0].paddle.rect.y > GAME_AREA_HEIGHT - PADDLE_HEIGHT) {
            pong->players[0].paddle.rect.y = GAME_AREA_HEIGHT - PADDLE_HEIGHT;
        }
    }

    if (pong->players[1].type == PLAYER_TYPE_HUMAN) {
        if (cara_key_down(pong->cara, KEY_UP)) {
            pong->players[1].paddle.rect.y -= PADDLE_SPEED * delta_time;
        }
        if (cara_key_down(pong->cara, KEY_DOWN)) {
            pong->players[1].paddle.rect.y += PADDLE_SPEED * delta_time;
        }
        if (pong->players[1].paddle.rect.y < 0) {
            pong->players[1].paddle.rect.y = 0;
        }
        if (pong->players[1].paddle.rect.y > GAME_AREA_HEIGHT - PADDLE_HEIGHT) {
            pong->players[1].paddle.rect.y = GAME_AREA_HEIGHT - PADDLE_HEIGHT;
        }
    }

	return CARA_STATUS_OK;
}

static cara_status_t pong_render(
	cara_app_t *app,
	const cara_pixbuf_t **p_out_buffer
)
{
	pong_t *pong = app->data;

	pong_render_scoreboard(pong);
	pong_render_gameboard(pong);

	*p_out_buffer = &pong->renderer.framebuffer;

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
    pong_t *pong = app->data;
    renderer_t *renderer = &pong->renderer;
    cara_pixbuf_t *framebuffer = &renderer->framebuffer;

    free(framebuffer->pixels);

	return CARA_STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////////
// Render functions
//////////////////////////////////////////////////////////////////////////////

static void pong_render_scoreboard(pong_t *pong)
{
    pong_render_fill_area(
        &pong->renderer,
        AREA_SCOREBOARD,
        BACKGROUDN_COLOR_SCOREBOARD
    );
}

static void pong_render_gameboard(pong_t *pong)
{
    pong_render_fill_area(
        &pong->renderer,
        AREA_GAME,
        BACKGROUND_COLOR_GAME
    );

    pong_render_rect(
        &pong->renderer,
        AREA_GAME,
        &pong->players[0].paddle.rect,
        PADDLE_COLOR
    );

    pong_render_rect(
        &pong->renderer,
        AREA_GAME,
        &pong->players[1].paddle.rect,
        PADDLE_COLOR
    );

    pong_render_rect(
        &pong->renderer,
        AREA_GAME,
        &pong->ball.rect,
        BALL_COLOR
    );
}

static void pong_render_rect(
    renderer_t *renderer,
    area_idx_t area_idx,
    const rect_t *rect,
    uint32_t color
)
{
    cara_pixbuf_t *framebuffer = &renderer->framebuffer;
    layout_t *layout = &renderer->layouts[area_idx];
    viewport_t *viewport = &layout->viewport;

    double left = fmax(rect->x, 0.0);
    double top = fmax(rect->y, 0.0);
    double right = fmin(rect->x + rect->width, layout->area.width);
    double bottom = fmin(rect->y + rect->height, layout->area.height);

    if (right <= left || bottom <= top) {
        return;
    }

    uint32_t x0 = viewport->x + (uint32_t)(left * layout->scale_x);
    uint32_t y0 = viewport->y + (uint32_t)(top * layout->scale_y);
    uint32_t x1 = viewport->x + (uint32_t)ceil(right * layout->scale_x);
    uint32_t y1 = viewport->y + (uint32_t)ceil(bottom * layout->scale_y);

    for (uint32_t y = y0; y < y1; ++y) {
        for (uint32_t x = x0; x < x1; ++x) {
            framebuffer->pixels[y * framebuffer->width + x] = color;
        }
    }
}

static void pong_render_fill_area(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t color
)
{
    cara_pixbuf_t *framebuffer = &renderer->framebuffer;
    viewport_t *viewport = &renderer->layouts[area_idx].viewport;

    uint32_t x0 = viewport->x;
    uint32_t y0 = viewport->y;
    uint32_t x1 = x0 + viewport->width;
    uint32_t y1 = y0 + viewport->height;

    for (uint32_t y = y0; y < y1; ++y) {
        for (uint32_t x = x0; x < x1; ++x) {
            framebuffer->pixels[y * framebuffer->width + x] = color;
        }
    }
}