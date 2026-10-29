#include <stdlib.h>
#include <stdio.h>

#include "cara/cara.h"

typedef struct {
    unsigned int update_count;
    cara_t *cara;
} app_data_t;


static cara_status_t app_init(cara_app_t *app) {
    app_data_t *data = app->data;

    data->update_count = 0;

    fprintf(stdout, "Application initialized.\n");

    return CARA_STATUS_OK;
}

static cara_status_t app_event(
    cara_app_t *app,
    cara_event_t event
)
{
    (void) app;
    (void) event;
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
    *out_buffer = NULL;
    fprintf(stdout, "Update count: %u\n", data->update_count);

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