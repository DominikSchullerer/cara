#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cara/cara.h"

#define RESOLUTION_X    600
#define RESOLUTION_Y    800
#define UPS             120
#define FPS             30

typedef struct {
    unsigned int update_count;
    cara_t *cara;
    cara_pixbuf_t pixbuf;
    uint32_t rect[4];
    uint32_t speed;
} app_data_t;


static cara_status_t app_init(cara_app_t *app) {
    app_data_t *data = app->data;
    cara_pixbuf_t *pixbuf = &data->pixbuf;

    data->update_count = 0;
    data->pixbuf.width = RESOLUTION_X;
    data->rect[0] = 0;
    data->rect[1] = 0;
    data->rect[2] = 20;
    data->rect[3] = 20;
    data->speed = 0;
    pixbuf->height = RESOLUTION_Y;
    pixbuf->pixels = calloc(RESOLUTION_X * RESOLUTION_Y, sizeof(*pixbuf->pixels));

    fprintf(stdout, "Application initialized.\n");

    return CARA_STATUS_OK;
}

static cara_status_t app_event(
    cara_app_t *app,
    cara_event_t event
)
{
    app_data_t *data = app->data;

    switch (event.type) {
        case CARA_RESIZED:
            fprintf(stdout, "Window resized: %u x %u\n",
                    event.resize.new_width, event.resize.new_height);
            break;
        case CARA_CLOSE_REQUEST:
            cara_request_quit(data->cara);
            break;
    }
    return CARA_STATUS_OK;
}

static cara_status_t app_update(
    cara_app_t *app,
    double delta_time
) {
    app_data_t *data = app->data;

    (void) delta_time;

    data->update_count += 1;
    if (cara_key_pressed(data->cara, KEY_W)) {
        fprintf(stdout, "KEY_W pressed\n");
    }
    if (cara_key_down(data->cara, KEY_W)) {
        fprintf(stdout, "KEY_W down\n");
    }
    if (cara_key_released(data->cara, KEY_W)) {
        fprintf(stdout, "KEY_W released\n");
    }

    if (cara_key_pressed(data->cara, KEY_PLUS)) {
        data->speed += 1;
    }
    if (cara_key_pressed(data->cara, KEY_MINUS)) {
        data->speed -= 1;
    }

    if (cara_key_down(data->cara, KEY_W)) {
        data->rect[1] -= (uint32_t)(data->speed * delta_time * UPS);
        data->rect[3] -= (uint32_t)(data->speed * delta_time * UPS);
    }
    if (cara_key_down(data->cara, KEY_S)) {
        data->rect[1] += (uint32_t)(data->speed * delta_time * UPS);
        data->rect[3] += (uint32_t)(data->speed * delta_time * UPS);
    }
    if (cara_key_down(data->cara, KEY_A)) {
        data->rect[0] -= (uint32_t)(data->speed * delta_time * UPS);
        data->rect[2] -= (uint32_t)(data->speed * delta_time * UPS);
    }
    if (cara_key_down(data->cara, KEY_D)) {
        data->rect[0] += (uint32_t)(data->speed * delta_time * UPS);
        data->rect[2] += (uint32_t)(data->speed * delta_time * UPS);
    }

    return CARA_STATUS_OK;
}

static cara_status_t app_render(
    cara_app_t *app,
    const cara_pixbuf_t **out_buffer
) {
    app_data_t *data = app->data;
    cara_pixbuf_t *pixbuf = &data->pixbuf;
    *out_buffer = NULL;
    fprintf(stdout, "Update count: %u\n", data->update_count);

    memset(pixbuf->pixels, 0x80, RESOLUTION_X * RESOLUTION_Y * sizeof(*pixbuf->pixels));

    for (uint32_t y = data->rect[1]; y < data->rect[3]; y++) {
        for (uint32_t x = data->rect[0]; x < data->rect[2]; x++) {
            pixbuf->pixels[(y % RESOLUTION_Y) * pixbuf->width + (x % RESOLUTION_X)] = 0x00FF0000;
        }
    }

    *out_buffer = pixbuf;

    return CARA_STATUS_OK;
}

static void app_shutdown(cara_app_t *app) {
    app_data_t *data = app->data;

    fprintf(
        stdout,
        "Application stopped after %u updates.\n",
        data->update_count
    );
}

int main(void) {
    cara_config_t config = {
        .ups = UPS,
        .fps = FPS,
        .window.title = "ExampleWindow",
        .window.width = RESOLUTION_X,
        .window.height = RESOLUTION_Y
    };

    cara_t *cara = NULL;

    cara_status_t status = cara_create(&config, &cara);

    if (status != CARA_STATUS_OK) {
        fprintf(stderr, "Failed to create CARA: %d\n", status);
        return EXIT_FAILURE;
    }

    app_data_t data = {0};
    data.cara = cara;

    cara_app_t app = {
        .data = &data,
        .init = app_init,
        .event = app_event,
        .update = app_update,
        .render = app_render,
        .shutdown = app_shutdown
    };

    status = cara_run(cara, &app);

    cara_destroy(cara);

    if (status != CARA_STATUS_OK) {
        fprintf(stderr, "CARA exited with status: %d\n", status);
        return 1;
    }

    return EXIT_SUCCESS;
}