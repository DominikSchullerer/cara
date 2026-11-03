#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "cara_core/cara_core.h"
#include "cara_platform/cara_platform.h"
#include "cara_event/cara_event_manager.h"

//****************************************************************************
// Types
//****************************************************************************

/**
 * @brief Represents a time-controlled execution channel.
 */
typedef struct cara_timer_channel_t {
	// Target interval in seconds.
	double dt;
	// Accumulated elapsed time in seconds.
	double accumulator;
} cara_timer_channel_t;

/**
 * @brief Maintains timing information for the main loop.
 */
typedef struct cara_timer_t {
	double prev_time;
	double cur_time;
	double dt;

	cara_timer_channel_t update;
	cara_timer_channel_t render;
} cara_timer_t;

/**
 * @brief Internal state of the core subsystem
 */
struct cara_core_t {
	// Owning runtime instance
	cara_t *cara;

	bool is_running;
	bool quit_requested;

	cara_timer_t timer;
};


//****************************************************************************
// Function Declarations
//****************************************************************************


static cara_status_t cara_core_init_timer(cara_core_t *core);
static cara_status_t cara_core_update_timer(cara_core_t *core);


static cara_status_t cara_core_start_app(cara_core_t *core, cara_app_t *app);
static void cara_core_shutdown_app(cara_core_t *core, cara_app_t *app);
static cara_status_t cara_core_do_update(cara_core_t *core, cara_app_t *app);
static cara_status_t cara_core_do_render(cara_core_t *core, cara_app_t *app);
static cara_status_t cara_core_do_mainloop(cara_core_t *core, cara_app_t *app);


//****************************************************************************
// API Functions
//****************************************************************************

cara_status_t cara_core_create(cara_t *cara, cara_core_t **p_core)
{
	*p_core = NULL;

	cara_core_t *core = calloc(1, sizeof(*core));
	if (!core) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	core->cara = cara;
	cara_status_t status = cara_core_init_timer(core);
	if (status != CARA_STATUS_OK) {
		goto error_exit;
	}

	*p_core = core;

	return CARA_STATUS_OK;

error_exit:
	cara_core_destroy(core);
	return status;
}

//-----------

void cara_core_destroy(cara_core_t *core)
{
	if (!core) {
		return;
	}

	free(core);
}

//-----------

void cara_core_request_quit(cara_core_t *core)
{
	core->quit_requested = true;
}

//-----------

cara_status_t cara_core_run(cara_core_t *core, cara_app_t *app)
{
	if (core->is_running) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	cara_status_t status = cara_core_start_app(core, app);
	if (status != CARA_STATUS_OK) {
		core->is_running = false;
		return status;
	}

	status = cara_core_do_mainloop(core, app);

	cara_core_shutdown_app(core, app);

	return status;
}

//****************************************************************************
// Timer
//****************************************************************************

/**
 * @brief			Initializes the core timer.
 * @param core		Core instance.
 * @return			CARA_STATUS_OK on success.
 */
static cara_status_t cara_core_init_timer(cara_core_t *core)
{
	cara_timer_t *timer = &core->timer;
	cara_platform_t *platform = core->cara->platform;
	cara_config_t *config = &core->cara->config;

	cara_status_t status = cara_platform_get_time(platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->prev_time = timer->cur_time;
	timer->dt = 0.0;
	timer->update.accumulator = 0.0;
	timer->render.accumulator = 0.0;

	timer->update.dt = config->ups > 0
		? 1.0 / (double) config->ups
		: 0.0;
	timer->render.dt = config->fps > 0
		? 1.0 / (double) config->fps
		: 0.0;

	return CARA_STATUS_OK;
}

//-----------

/**
 * @brief			Updates timestamps and time accumulators.
 * @param core		Core instance.
 * @return			CARA_STATUS_OK on success.
 */
static cara_status_t cara_core_update_timer(cara_core_t *core)
{
	cara_timer_t *timer = &core->timer;
	cara_platform_t *platform = core->cara->platform;
	cara_status_t status = cara_platform_get_time(platform, &timer->cur_time);
	if (status != CARA_STATUS_OK) {
		return status;
	}

	timer->dt = timer->cur_time - timer->prev_time;
	timer->prev_time = timer->cur_time;

	timer->update.accumulator += timer->dt;
	timer->render.accumulator += timer->dt;

	return CARA_STATUS_OK;
}

//******************************************************************************
// Run
//******************************************************************************

/**
 * @brief			Sets core to running and calls the init-callback.
 * @param core		Core instance.
 * @param app		Application whose init-callback is executed.
 * @return			CARA_STATUS_OK on success.
 */
static cara_status_t cara_core_start_app(
	cara_core_t *core,
	cara_app_t *app
) {
	core->is_running = true;
	core->quit_requested = false;

	return app->init(app);
}

//-----------

/**
 * @brief			Calls the shutdown-callback and sets core to be not running.
 * @param core		Core instance.
 * @param app		Application whose shutdown-callback is executed.
 */
static void cara_core_shutdown_app(
	cara_core_t *core,
	cara_app_t *app
) {
	app->shutdown(app);
	core->is_running = false;
}

//-----------

/**
 * @brief			Executes the update-callback according to its timing.
 * @param core		Core instance.
 * @param app		Application whose update-callback is executed.
 * @return			CARA_STATUS_OK on success.
 */
static cara_status_t cara_core_do_update(cara_core_t *core, cara_app_t *app)
{
	cara_timer_t *timer = &core->timer;
	if (timer->update.dt == 0) {
		cara_status_t status = app->update(app, timer->update.accumulator);
		if (status != CARA_STATUS_OK) {
			return status;
		}

		timer->update.accumulator = 0.0;
	}
	else {
		while (timer->update.accumulator >= timer->update.dt) {
			if (core->quit_requested) {
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

//-----------

/**
 * @brief			Executes the render-callback according to its timing and draws to screen if necessary.
 * @param core		Core instance.
 * @param app		Application whose render-callback is executed.
 * @return			CARA_STATUS_OK on success.
 */
static cara_status_t cara_core_do_render(cara_core_t *core, cara_app_t *app) 
{
	cara_timer_t *timer = &core->timer;
	cara_platform_t *platform = core->cara->platform;
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
		status = cara_platform_present(platform, pixbuf);
		if (status != CARA_STATUS_OK) {
			return status;
		}
	}

	return status;
}

//-----------

/**
 * @brief			Executes the mainloop (event-handling, updates, rendering).
 * @param core		Core instances.
 * @param app		Application whose callbacks are executed.
 * @return			CARA_STATUS_OK on successfull execution.
 */
static cara_status_t cara_core_do_mainloop(cara_core_t *core, cara_app_t *app)
{
	cara_status_t status = CARA_STATUS_OK;
	cara_event_manager_t *event_manager = core->cara->event_manager;
	cara_platform_t *platform = core->cara->platform;

	status = cara_core_init_timer(core);
	if (status != CARA_STATUS_OK) {
		return status;
	}


	while (!core->quit_requested) {
		status = cara_platform_process_messages(platform);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}

		status = cara_event_process(event_manager, app);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}

		status = cara_core_update_timer(core);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}

		status = cara_core_do_update(core, app);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}

		status = cara_core_do_render(core, app);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}

		status = cara_platform_wait(platform, 0.001);
		if (status != CARA_STATUS_OK || core->quit_requested) {
			return status;
		}
	}

	return status;
}
