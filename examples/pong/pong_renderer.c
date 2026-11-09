#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "cara/cara.h"

#include "pong_constants.h"
#include "pong_types.h"


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


void pong_render_scoreboard(pong_t *pong)
{
    pong_render_fill_area(
        &pong->renderer,
        AREA_SCOREBOARD,
        BACKGROUDN_COLOR_SCOREBOARD
    );
}


void pong_render_gameboard(pong_t *pong)
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