#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "cara/cara.h"

#include "pong_constants.h"
#include "pong_types.h"
#include "pong_renderer.h"
#include "pong_update.h"

//////////////////////////////////////////////////////////////////////////////
// Callback declarations
//////////////////////////////////////////////////////////////////////////////

static cara_status_t pong_init(cara_app_t *app);

static cara_status_t pong_event(
	cara_app_t *app, 
	cara_event_t event
);

static cara_status_t pong_shutdown(cara_app_t *app);

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main(void)
{
	pong_t pong = {0};
	srand((unsigned int)time(NULL));

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

    pong->players[0].paddle.rect.upper_left.x = PADDLE_OFFSET;
    pong->players[0].paddle.rect.upper_left.y = PADDLE_START_Y;
    pong->players[0].paddle.rect.width = PADDLE_WIDTH;
    pong->players[0].paddle.rect.height = PADDLE_HEIGHT;

    pong->players[1].paddle.rect.upper_left.x = GAME_AREA_WIDTH - PADDLE_OFFSET;
    pong->players[1].paddle.rect.upper_left.y = PADDLE_START_Y;
    pong->players[1].paddle.rect.width = PADDLE_WIDTH;
    pong->players[1].paddle.rect.height = PADDLE_HEIGHT;

    pong->ball.rect.width = BALL_WIDTH;
    pong->ball.rect.height = BALL_HEIGHT;
	pong->ball.rect.upper_left.x = BALL_START_X;
	pong->ball.rect.upper_left.y = BALL_START_Y;

    pong->control.player_controls[0].up = KEY_W;
    pong->control.player_controls[0].down = KEY_S;
    pong->control.player_controls[1].up = KEY_UP;
    pong->control.player_controls[1].down = KEY_DOWN;
    pong->control.exit = KEY_ESCAPE;
	pong->control.start = KEY_ENTER;

	pong->is_running = false;

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
            break;
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

