#include <stdlib.h>
#include <stdbool.h>

#include "cara_event/cara_event_manager.h"
#include "cara/cara_app.h"

/**
 * @brief Stores an event.
 */
typedef struct cara_event_container_t {
	bool exists;
	cara_event_t event;
} cara_event_container_t;

/**
 * @brief Internal state of the event manager.
 */
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

void cara_event_manager_destroy(cara_event_manager_t *event_manager)
{
	if (!event_manager) {
		return;
	}

	free(event_manager);
}

cara_status_t cara_event_insert(
	cara_event_manager_t *event_manager,
	cara_event_t event
)
{
	switch (event.type) {
		case CARA_CLOSE_REQUEST:
			event_manager->close_request.exists = true;
			event_manager->close_request.event = event;
			break;
		case CARA_RESIZED:
			event_manager->resize.exists = true;
			event_manager->resize.event = event;
			break;
		default:
			return CARA_STATUS_INVALID_ARGUMENT;
	}

	return CARA_STATUS_OK;
}

cara_status_t cara_event_process(
	cara_event_manager_t *event_manager,
	cara_app_t *app
)
{
	cara_status_t status = CARA_STATUS_OK;

	if (event_manager->close_request.exists) {
		status = app->event(app, event_manager->close_request.event);
		if (status != CARA_STATUS_OK) {
			goto exit;
		}
	}

	if (event_manager->resize.exists) {
		status = app->event(app, event_manager->resize.event);
		if (status != CARA_STATUS_OK) {
			goto exit;
		}
	}

exit:
	event_manager->close_request.exists = 0;
	event_manager->resize.exists = 0;

	return status;
}
