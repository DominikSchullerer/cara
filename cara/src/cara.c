#include <stdlib.h>
#include <math.h>

#include "cara/cara.h"
#include "cara_internal.h"


typedef struct cara_timer_t cara_timer_t;
struct cara_timer_t {
	double prev_time;
	double cur_time;
	double dt;
	double update_dt;
	double update_accumulator;
	double render_dt;
	double render_accumulator;
};

static cara_status_t cara_init_timer(
	cara_t *cara, 
	cara_timer_t *timer
);

static cara_status_t cara_update_timer(
	cara_t *cara,
	cara_timer_t *timer
);

static cara_status_t cara_do_update(
	cara_t *cara,
	cara_app_t *app,
	cara_timer_t *timer
);

static cara_status_t cara_do_render(
	cara_t *cara,
	cara_app_t *app,
	cara_timer_t *timer
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

	cara_status_t status = CARA_STATUS_OK;

	status = cara_platform_create(&cara->platform);
	if (status != CARA_STATUS_OK) {
		free(cara);
		return status;
	}

	cara->config = *config;
	cara->quit_requested = false;
	cara->running = false;

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

	cara->running = true;
	cara->quit_requested = false;


	status = app->init(app);
	if (status != CARA_STATUS_OK) {
		cara->running = false;
		return status;
	}

	cara_timer_t timer = {0};
	status = cara_init_timer(cara, &timer);
	if (status != CARA_STATUS_OK) {
		app->shutdown(app);
		cara->running = false;
		return status;
	}

	while (!cara->quit_requested) {
		status = cara_update_timer(cara, &timer);
		if (status != CARA_STATUS_OK) {
			break;
		}

		status = cara_do_update(cara, app, &timer);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			break;
		}

		status = cara_do_render(cara, app, &timer);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			break;
		}

		// status = cara_platform_wait(cara->platform, 0.001);
		if (status != CARA_STATUS_OK || cara->quit_requested) {
			break;
		}
	}

	cara->running = false;
	app->shutdown(app);

	return status;
}


static cara_status_t cara_init_timer(cara_t *cara, cara_timer_t *timer) 
{
	cara_status_t status = cara_platform_get_time(cara->platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->prev_time = timer->cur_time;
	timer->dt = 0.0;
	timer->update_accumulator = 0.0;
	timer->render_accumulator = 0.0;

	if (cara->config.ups == 0) {
		timer->update_dt = 0.0;
	}
	else {
		timer->update_dt = 1.0 / (double) cara->config.ups;
	}

	if (cara->config.fps == 0) {
		timer->render_dt = 0.0;
	}
	else {
		timer->render_dt = 1.0 / (double) cara->config.fps;
	}

	return status;
}

static cara_status_t cara_update_timer(
	cara_t *cara,
	cara_timer_t *timer
) {
	cara_status_t status = cara_platform_get_time(cara->platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->dt = timer->cur_time - timer->prev_time;
	timer->prev_time = timer->cur_time;

	timer->update_accumulator += timer->dt;
	timer->render_accumulator += timer->dt;

	return status;
}

static cara_status_t cara_do_update(
	cara_t *cara,
	cara_app_t *app,
	cara_timer_t *timer
) {

	if (timer->update_dt == 0) {
		cara_status_t status = app->update(app, timer->update_accumulator);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		timer->update_accumulator = 0.0;
	}
	else {
		while (timer->update_accumulator >= timer->update_dt) {
			if (cara->quit_requested) {
				return CARA_STATUS_OK;
			}

			cara_status_t status = app->update(app, timer->update_dt);
			if (status != CARA_STATUS_OK) {
				return status;
			}

			timer->update_accumulator -= timer->update_dt;
		}
	}

	return CARA_STATUS_OK;
}

static cara_status_t cara_do_render(
	cara_t *cara,
	cara_app_t *app,
	cara_timer_t *timer
) {
	(void) cara;
	cara_status_t status = CARA_STATUS_OK;
	cara_pixbuf_t *pixbuf = NULL;

	if (timer->render_dt == 0) {
		status = app->render(app, &pixbuf);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		timer->render_accumulator = 0.0;
	}
	else {
		if (timer->render_accumulator >= timer->render_dt) {
			status = app->render(app, &pixbuf);
			if (status != CARA_STATUS_OK) {
				return status;
			}

			timer->render_accumulator = fmod(timer->render_accumulator, timer->render_dt);
		}
	}

	if (pixbuf) {
		// ToDo: Draw to Screen
		;
	}

	return status;
}
