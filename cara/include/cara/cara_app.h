#ifndef CARA_APP_H_INCLUDED
#define CARA_APP_H_INCLUDED

#include "cara/cara_status.h"
#include "cara/cara_pixbuf.h"

typedef struct cara_app_t cara_app_t;


typedef cara_status_t (*cara_app_init_fn)(cara_app_t *app);

typedef cara_status_t (*cara_app_update_fn)(
    cara_app_t *app,
    double delta_time
);

typedef cara_status_t (*cara_app_render_fn)(
    cara_app_t *app,
    const cara_pixbuf_t **out_buffer
);

typedef void (*cara_app_shutdown_fn)(cara_app_t *app);


struct cara_app_t {
	void *data;

    cara_app_init_fn init;
    cara_app_update_fn update;
    cara_app_render_fn render;
    cara_app_shutdown_fn shutdown;
};

#endif // !CARA_APP_H_INCLUDED
