#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cara/cara.h"

#define RESOLUTION_X    240
#define RESOLUTION_Y    240

typedef struct {
    unsigned int update_count;
    cara_t *cara;
    cara_pixbuf_t pixbuf;
} app_data_t;


static cara_status_t app_init(cara_app_t *app) {
    app_data_t *data = app->data;
    cara_pixbuf_t *pixbuf = &data->pixbuf;

    data->update_count = 0;
    data->pixbuf.width = RESOLUTION_X;
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

    uint32_t rect_width = data->update_count % (RESOLUTION_X + 1);
    uint32_t rect_height = data->update_count % (RESOLUTION_Y + 1);

    for (uint32_t y = 0; y < rect_height; y++) {
        for (uint32_t x = 0; x < rect_width; x++) {
            pixbuf->pixels[y * pixbuf->width + x] = 0x00FF0000;
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
        .ups = 120,
        .fps = 30,
        .window.title = "ExampleWindow",
        .window.width = 640,
        .window.height = 480
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