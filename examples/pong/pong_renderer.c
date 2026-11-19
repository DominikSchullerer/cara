#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "cara/cara.h"

#include "pong_constants.h"
#include "pong_types.h"
#include "pong_digit_bitmaps.h"

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

static void pong_render_digit(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t digit,
    double x_offset,
    double y_offset,
    double size,
    uint32_t color
);

static void pong_render_number_left(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t number,
    double x,
    double y,
    double scale,
    double spacing,
    uint32_t color
);

static void render_number_right(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t number,
    double x,
    double y,
    double scale,
    double spacing,
    uint32_t color
);

//////////////////////////////////////////////////////////////////////////////

cara_status_t pong_render(
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

//////////////////////////////////////////////////////////////////////////////

static void pong_render_scoreboard(pong_t *pong)
{
    pong_render_fill_area(
        &pong->renderer,
        AREA_SCOREBOARD,
        BACKGROUND_COLOR_SCOREBOARD
    );

    pong_render_number_left(
        &pong->renderer,
        AREA_SCOREBOARD,
        pong->players[0].score,
        30.0,
        30.0,
        20.0,
        10.0,
        SCORE_COLOR
    );

    render_number_right(
        &pong->renderer,
        AREA_SCOREBOARD,
        pong->players[1].score,
        SCOREBOARD_WIDTH - 30.0,
        30.0,
        20.0,
        10.0,
        SCORE_COLOR
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

    double left = fmax(rect->upper_left.x, 0.0);
    double top = fmax(rect->upper_left.y, 0.0);
    double right = fmin(rect->upper_left.x + rect->width, layout->area.width);
    double bottom = fmin(rect->upper_left.y + rect->height, layout->area.height);

    if (right <= left || bottom <= top) {
        return;
    }

    uint32_t x0 = viewport->x + (uint32_t) (left * layout->scale_x);
    uint32_t y0 = viewport->y + (uint32_t) (top * layout->scale_y);
    uint32_t x1 = viewport->x + (uint32_t) ceil(right * layout->scale_x);
    uint32_t y1 = viewport->y + (uint32_t) ceil(bottom * layout->scale_y);

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

static void pong_render_digit(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t digit,
    double x_offset,
    double y_offset,
    double size,
    uint32_t color
)
{
    if (digit > 9) {
        return;
    }

    const char (*digit_pattern)[DIGIT_HEIGHT][DIGIT_WIDTH] = &DIGIT_BITMAPS[digit];

    for (uint32_t y = 0; y < DIGIT_HEIGHT; y++) {
        for (uint32_t x = 0; x < DIGIT_WIDTH; x++) {
            if ((*digit_pattern)[y][x]) {
                rect_t pixel_rect = {
                    .upper_left.x = x_offset + x * size,
                    .upper_left.y = y_offset + y * size,
                    .width = size,
                    .height = size
                };
                pong_render_rect(renderer, area_idx, &pixel_rect, color);
            }
        }
    }
}

static void pong_render_number_left(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t number,
    double x,
    double y,
    double scale,
    double spacing,
    uint32_t color
)
{
    uint8_t digits[4];
    uint32_t count = 0;

    do {
        digits[count] = number % 10;
        count += 1;
        number /= 10;
    } while (number > 0 && count < 4);

    double digit_advance = DIGIT_WIDTH * scale + spacing;

    for (uint32_t i = 0; i < count; ++i) {
        pong_render_digit(
            renderer,
            area_idx,
            digits[count - 1 - i],
            x + i * digit_advance,
            y,
            scale,
            color
        );
    }
}

static void render_number_right(
    renderer_t *renderer,
    area_idx_t area_idx,
    uint32_t number,
    double x,
    double y,
    double scale,
    double spacing,
    uint32_t color
)
{
    uint8_t digits[4];
    uint32_t count = 0;
    do {
        digits[count] = number % 10;
        count += 1;
        number /= 10;
    } while (number > 0 && count < 4);

    double digit_advance = DIGIT_WIDTH * scale + spacing;

    for (uint32_t i = 0; i < count; ++i) {
        pong_render_digit(
            renderer,
            area_idx,
            digits[count - 1 - i],
            x - (count - 1 - i) * digit_advance - DIGIT_WIDTH * scale,
            y,
            scale,
            color
        );
    }
}
