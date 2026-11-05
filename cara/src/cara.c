#include <stdlib.h>
#include <math.h>

#include "cara/cara.h"
#include "cara_internal/cara_type.h"
#include "cara_platform/cara_platform.h"
#include "cara_event/cara_event_manager.h"
#include "cara_core/cara_core.h"
#include "cara_input/cara_input.h"


cara_status_t cara_create(const cara_config_t *config, cara_t **p_cara)
{
	if (!p_cara) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}
	*p_cara = NULL;

	if (!config) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (config->window.title == NULL ||
		config->window.height == 0 || 
		config->window.width == 0) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	cara_t *cara = calloc(1, sizeof(*cara));
	if (!cara) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	cara_status_t status = CARA_STATUS_OK;

	status = cara_event_manager_create(&cara->event_manager);
	if (status != CARA_STATUS_OK) {
		cara_destroy(cara);
		return status;
	}

	status = cara_platform_create(cara, &config->window, &cara->platform);
	if (status != CARA_STATUS_OK) {
		cara_destroy(cara);
		return status;
	}

	status = cara_core_create(cara, &cara->core);
	if (status != CARA_STATUS_OK) {
		cara_destroy(cara);
		return status;
	}

	status = cara_input_create(cara, &cara->input);
	if (status != CARA_STATUS_OK) {
		cara_destroy(cara);
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

	cara_input_destroy(cara->input);
	cara_core_destroy(cara->core);
	cara_event_manager_destroy(cara->event_manager);
	cara_platform_destroy(cara->platform);

	free(cara);
}

void cara_request_quit(cara_t *cara)
{
	if (cara == NULL) {
		return;
	}

	cara_core_request_quit(cara->core);
}

cara_status_t cara_run(cara_t *cara, cara_app_t *app)
{
	if (!cara || !app) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (!app->init || !app->update || !app->render || !app->shutdown || !app->event) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	return cara_core_run(cara->core, app);
}

bool cara_key_down(cara_t *cara, cara_key_t key)
{
	if (!cara || key < 0) {
		return false;
	}

	return cara_input_key_down(cara->input, key);
}

bool cara_key_pressed(cara_t *cara, cara_key_t key)
{
	if (!cara || key < 0) {
		return false;
	}

	return cara_input_key_pressed(cara->input, key);
}

bool cara_key_released(cara_t *cara, cara_key_t key)
{
	if (!cara || key < 0) {
		return false;
	}

	return cara_input_key_released(cara->input, key);
}
