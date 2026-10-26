#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "cara/cara.h"
#include "cara_internal.h"


static cara_status_t cara_start_app(
	cara_t *cara,
	cara_app_t *app
);

static void cara_shutdown_app(
	cara_t *cara,
	cara_app_t *app
);

static cara_status_t cara_init_timer(
	cara_t *cara
);

static cara_status_t cara_update_timer(
	cara_t *cara
);

static cara_status_t cara_do_update(
	cara_t *cara,
	cara_app_t *app
);

static cara_status_t cara_do_render(
	cara_t *cara,
	cara_app_t *app
);

static cara_status_t cara_do_mainloop(
	cara_t *cara,
	cara_app_t *app
);


cara_status_t cara_create(const cara_config_t *config, cara_t **p_cara)
{
	if (!p_cara) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}
	*p_cara = NULL;

	if (!config) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	cara_t *cara = calloc(1, sizeof(*cara));
	if (!cara) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	cara_status_t status = cara_platform_create(&cara->platform);
	if (status != CARA_STATUS_OK) {
		free(cara);
		return status;
	}

	cara->config = *config;

	*p_cara = cara;

	return CARA_STATUS_OK;
}

void cara_destroy(cara_t *cara)
{
	if (!cara) {
		return;
	}

	cara_platform_destroy(cara->platform);
	free(cara);
}

void cara_request_quit(cara_t *cara)
{
	if (cara == NULL) {
		return;
	}

	cara->quit_requested = true;
}

cara_status_t cara_run(cara_t *cara, cara_app_t *app)
{
	if (!cara || !app) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (cara->running) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (!app->init || !app->update || !app->render || !app->shutdown) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	cara_status_t status = CARA_STATUS_OK;

	status = cara_start_app(cara, app);
	if (status != CARA_STATUS_OK) {
		cara->running = false;
		return status;
	}

	status = cara_do_mainloop(cara, app);

	cara_shutdown_app(cara, app);

	return status;
}


static cara_status_t cara_start_app(
	cara_t *cara,
	cara_app_t *app
) {
	cara->running = true;
	cara->quit_requested = false;

	return app->init(app);
}

static void cara_shutdown_app(
	cara_t *cara,
	cara_app_t *app
) {
	cara->running = false;
	app->shutdown(app);
}


static cara_status_t cara_init_timer(cara_t *cara) 
{
	cara_timer_t *timer = &cara->timer;
	cara_status_t status = cara_platform_get_time(cara->platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->prev_time = timer->cur_time;
	timer->dt = 0.0;
	timer->update.accumulator = 0.0;
	timer->render.accumulator = 0.0;

	timer->update.dt = cara->config.ups > 0
		? 1.0 / (double) cara->config.ups
		: 0.0;
	timer->render.dt = cara->config.fps > 0
		? 1.0 / (double) cara->config.fps
		: 0.0;

	return CARA_STATUS_OK;
}

static cara_status_t cara_update_timer(cara_t *cara) 
{
	cara_timer_t *timer = &cara->timer;
	cara_status_t status = cara_platform_get_time(cara->platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->dt = timer->cur_time - timer->prev_time;
	timer->prev_time = timer->cur_time;

	timer->update.accumulator += timer->dt;
	timer->render.accumulator += timer->dt;

	return CARA_STATUS_OK;
}

static cara_status_t cara_do_update(
	cara_t *cara,
	cara_app_t *app
) {
	cara_timer_t *timer = &cara->timer;
	if (timer->update.dt == 0) {
		cara_status_t status = app->update(app, timer->update.accumulator);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		timer->update.accumulator = 0.0;
	}
	else {
		while (timer->update.accumulator >= timer->update.dt) {
			if (cara->quit_requested) {
				return CARA_STATUS_OK;
			}

			cara_status_t status = app->update(app, timer->update.dt);
			if (status != CARA_STATUS_OK) {
				return status;
			}

			timer->update.accumulator -= timer->update.dt;
		}
	}

	return CARA_STATUS_OK;
}

static cara_status_t cara_do_render(
	cara_t *cara,
	cara_app_t *app
) {
	cara_timer_t *timer = &cara->timer;
	cara_status_t status = CARA_STATUS_OK;
	cara_pixbuf_t *pixbuf = NULL;

	if (timer->render.dt == 0) {
		status = app->render(app, &pixbuf);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		timer->render.accumulator = 0.0;
	}
	else {
		if (timer->render.accumulator >= timer->render.dt) {
			status = app->render(app, &pixbuf);
			if (status != CARA_STATUS_OK) {
				return status;
			}

			timer->render.accumulator = fmod(timer->render.accumulator, timer->render.dt);
		}
	}

	if (pixbuf) {
		// ToDo: Draw to Screen
		;
	}

	return status;
}

static cara_status_t cara_do_mainloop(
	cara_t *cara,
	cara_app_t *app
) {
	cara_status_t status = CARA_STATUS_OK;

	status = cara_init_timer(cara);
	if (status != CARA_STATUS_OK) {
		return status;
	}


	while (!cara->quit_requested) {
		status = cara_update_timer(cara);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		status = cara_do_update(cara, app);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			return status;
		}

		status = cara_do_render(cara, app);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			return status;
		}

		status = cara_platform_wait(cara->platform, 0.001);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			return status;
		}
	}

	return status;
}
