#include <stdlib.h>

#include "cara/cara.h"
#include "cara_internal.h"

cara_status_t cara_create(const cara_config_t *config, cara_t **p_cara)
{
	if (!config || !p_cara) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}
	*p_cara = NULL;

	cara_t *cara = calloc(1, sizeof(*cara));
	if (!cara) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	cara->config = *config;
	cara->quit_requested = false;
	cara->running = false;

	*p_cara = cara;

	return CARA_STATUS_OK;
}

void cara_destroy(cara_t *cara)
{
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

	if (!app->init || !app->update) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	cara->running = true;
	cara->quit_requested = false;

	cara_status_t status = app->init(app);
	if (status != CARA_STATUS_OK) {
		cara->running = false;
		return status;
	}

	while (!cara->quit_requested) {
		status = app->update(app, 1.00f / 60.0f);

		if (status != CARA_STATUS_OK) {
			break;
		}
	}

	cara->running = false;

	if (app->shutdown) {
		app->shutdown(app);
	}

	return status;
}