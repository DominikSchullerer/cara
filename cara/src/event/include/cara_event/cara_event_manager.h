#ifndef CARA_EVENT_MANAGER_H_INCLUDED
#define CARA_EVENT_MANAGER_H_INCLUDED

#include "cara/cara_status.h"
#include "cara/cara_event.h"
#include "cara/cara_app.h"
#include "cara_event_manager_type.h"

/**
 * @brief	Creates a new event manager.
 * @param p_event_manager	Receives the created event manager instance.
 * @return	CARA_STATUS_OK on success.
 */
cara_status_t cara_event_manager_create(cara_event_manager_t **p_event_manager);

/**
 * @brief Destroys an event manager.
 * @param event_manger Event manager to destroy.
 */
void cara_event_manager_destroy(cara_event_manager_t *event_manger);

/**
 * @brief Inserts an event into the event manager
 * @param event_manager Event manager receiving the event.
 * @param event Event to insert.
 * @return CARA_STATUS_OK on success.
 */
cara_status_t cara_event_insert(
	cara_event_manager_t *event_manager,
	cara_event_t event
);

/**
 * @brief Processes all pending events.
 * @param event_manager Event manager containing pending events.
 * @param app Application receiving the event callbacks.
 * @return CARA_STATUS_OK if all events were processed successfully.
 */
cara_status_t cara_event_process(
	cara_event_manager_t *event_manager,
	cara_app_t *app
);

#endif // !CARA_EVENT_MANAGER_H_INCLUDED
