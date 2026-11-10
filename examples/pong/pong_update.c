#include <stdlib.h>
#include <stdint.h>

#include "pong_types.h"
#include "pong_update.h"
#include "pong_constants.h"

//////////////////////////////////////////////////////////////////////////////

static void handle_player_input(pong_t *pong, size_t player_id, double delta_time);
static void ball_move(pong_t *pong, double delta_time);
static bool rects_collide(rect_t *rect_1, rect_t *rect_2);


//////////////////////////////////////////////////////////////////////////////

cara_status_t pong_update(
    cara_app_t *app,
    double delta_time
)
{
    pong_t *pong = app->data;

    if (cara_key_down(pong->cara, pong->control.exit)) {
        cara_request_quit(pong->cara);
        return CARA_STATUS_OK;
    }

    handle_player_input(pong, 0, delta_time);
    handle_player_input(pong, 1, delta_time);

    ball_move(pong, delta_time);

    return CARA_STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////////


static void handle_player_input(pong_t *pong, size_t player_id, double delta_time)
{
    player_control_t *player_control = &pong->control.player_controls[player_id];
    player_t *player = &pong->players[player_id];

    if (cara_key_down(pong->cara, player_control->up)) {
        player->paddle.rect.upper_left.y -= PADDLE_SPEED * delta_time;
    }
    if (cara_key_down(pong->cara, player_control->down)) {
        player->paddle.rect.upper_left.y += PADDLE_SPEED * delta_time;
    }
    if (player->paddle.rect.upper_left.y < 0) {
        player->paddle.rect.upper_left.y = 0;
    }
    if (player->paddle.rect.upper_left.y > GAME_AREA_HEIGHT - PADDLE_HEIGHT) {
        player->paddle.rect.upper_left.y = GAME_AREA_HEIGHT - PADDLE_HEIGHT;
    }
}

//////////////////////////////////////////////////////////////////////////////

// ToDo: use physics
static void ball_move(pong_t *pong, double delta_time)
{
    pong->ball.rect.upper_left.x += pong->ball.velocity.x * delta_time;
    pong->ball.rect.upper_left.y += pong->ball.velocity.y * delta_time;

    if (pong->ball.rect.upper_left.y < 0) {
        pong->ball.rect.upper_left.y = 0;
        pong->ball.velocity.y *= -1;
    }
    if (pong->ball.rect.upper_left.y > GAME_AREA_HEIGHT - BALL_HEIGHT) {
        pong->ball.rect.upper_left.y = GAME_AREA_HEIGHT - BALL_HEIGHT;
        pong->ball.velocity.y *= -1;
    }

    if (rects_collide(&pong->ball.rect, &pong->players[0].paddle.rect)) {
        pong->ball.velocity.x *= -1;
        pong->ball.rect.upper_left.x = pong->players[0].paddle.rect.upper_left.x + pong->players[0].paddle.rect.width;
    }
    if (rects_collide(&pong->ball.rect, &pong->players[1].paddle.rect)) {
        pong->ball.velocity.x *= -1;
        pong->ball.rect.upper_left.x = pong->players[1].paddle.rect.upper_left.x - pong->ball.rect.width;
    }

    // ToDo: Remove and add Scoring logic /
    if (pong->ball.rect.upper_left.x < 0) {
        pong->ball.rect.upper_left.x = 0;
        pong->ball.velocity.x *= -1;
    }
    if (pong->ball.rect.upper_left.x > GAME_AREA_WIDTH - BALL_WIDTH) {
        pong->ball.rect.upper_left.x = GAME_AREA_WIDTH - BALL_WIDTH;
        pong->ball.velocity.x *= -1;
    }
}

// ToDo: Use physics
static bool rects_collide(rect_t *rect_1, rect_t *rect_2)
{
    if (rect_1->upper_left.x > rect_2->upper_left.x + rect_2->width) {
        return false;
    }
    if (rect_1->upper_left.x + rect_1->width < rect_2->upper_left.x) {
        return false;
    }
    if (rect_1->upper_left.y > rect_2->upper_left.y + rect_2->height) {
        return false;
    }
    if (rect_1->upper_left.y + rect_1->height < rect_2->upper_left.y) {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////

