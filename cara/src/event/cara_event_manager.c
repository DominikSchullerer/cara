#include <stdlib.h>
#include <stdbool.h>

#include "event/include/cara_event_manager.h"
#include "cara/cara_event.h"

typedef struct cara_event_container_t cara_event_container_t;

struct cara_event_container_t {
	bool exists;
	cara_event_t event;
};

struct cara_event_manager_t {
	cara_event_container_t close_request;
	cara_event_container_t resize;
};

cara_status_t cara_event_manager_create(cara_event_manager_t **p_event_manager)
{
	cara_event_manager_t *event_manager = calloc(1, sizeof(*event_manager));
	if (!event_manager) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	*p_event_manager = event_manager;

	return CARA_STATUS_OK;
}

void cara_event_manager_destroy(cara_event_manager_t *event_manger)
{
	if (!event_manger) {
		return;
	}

	free(event_manger);
}

cara_status_t cara_event_reset(cara_event_manager_t *event_manager)
{
	if (!event_manager) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	event_manager->close_request.exists = 0;
	event_manager->resize.exists = 0;

	return CARA_STATUS_OK;
}

cara_status_t cara_event_insert(
	cara_event_manager_t *event_manager,
	cara_event_t event
)
{
	if (!event_manager) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	switch (event.type) {
		case CARA_CLOSE_REQUEST:
			event_manager->close_request.exists = true;
			event_manager->close_request.event = event;
			break;
		case CARA_RESIZED:
			event_manager->resize.exists = true;
			event_manager->resize.event = event;
			break;
	}

	return CARA_STATUS_OK;
}

cara_status_t cara_event_process(
	cara_event_manager_t *event_manager,
	cara_app_t *app
)
{
	if (!event_manager || !app) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

	if (event_manager->close_request.exists) {
		cara_status_t status = app->event(app, event_manager->close_request.event);
		if (status != CARA_STATUS_OK) {
			return status;
		}
	}

	if (event_manager->resize.exists) {
		cara_status_t status = app->event(app, event_manager->resize.event);
		if (status != CARA_STATUS_OK) {
			return status;
		}
	}

	return CARA_STATUS_OK;
}
