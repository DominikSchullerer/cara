#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "cara/cara.h"

#include "pong_constants.h"
#include "pong_types.h"
#include "pong_renderer.h"

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

static void ball_move(pong_t *pong, double_t delta_time);
static bool rects_collide(rect_t *rect_1, rect_t *rect_2);

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

    // ToDo: better starting velocity
    pong->ball.velocity.x = 300;
    pong->ball.velocity.y = 400;

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

    ball_move(pong, delta_time);

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

// ToDo: reflect instead of set to 0
static void ball_move(pong_t *pong, double_t delta_time)
{
    pong->ball.rect.x += pong->ball.velocity.x * delta_time;
    pong->ball.rect.y += pong->ball.velocity.y * delta_time;

    if (pong->ball.rect.y < 0) {
        pong->ball.rect.y = 0;
        pong->ball.velocity.y *= -1;
    }
    if (pong->ball.rect.y > GAME_AREA_HEIGHT - BALL_HEIGHT) {
        pong->ball.rect.y = GAME_AREA_HEIGHT - BALL_HEIGHT;
        pong->ball.velocity.y *= -1;
    }

    if (rects_collide(&pong->ball.rect, &pong->players[0].paddle.rect)) {
        pong->ball.velocity.x *= -1;
        pong->ball.rect.x = pong->players[0].paddle.rect.x + pong->players[0].paddle.rect.width;
    }
    if (rects_collide(&pong->ball.rect, &pong->players[1].paddle.rect)) {
        pong->ball.velocity.x *= -1;
        pong->ball.rect.x = pong->players[1].paddle.rect.x - pong->ball.rect.width;
    }

    // ToDo: Remove and add Scoring logic /
    if (pong->ball.rect.x < 0) {
        pong->ball.rect.x = 0;
        pong->ball.velocity.x *= -1;
    }
    if (pong->ball.rect.x > GAME_AREA_WIDTH - BALL_WIDTH) {
        pong->ball.rect.x = GAME_AREA_WIDTH - BALL_WIDTH;
        pong->ball.velocity.x *= -1;
    }
}


static bool rects_collide(rect_t *rect_1, rect_t *rect_2)
{
    if (rect_1->x > rect_2->x + rect_2->width) {
        return false;
    }
    if (rect_1->x + rect_1->width < rect_2->x) {
        return false;
    }
    if (rect_1->y > rect_2->y + rect_2->height) {
        return false;
    }
    if (rect_1->y + rect_1->height < rect_2->y) {
        return false;
    }

    return true;
}