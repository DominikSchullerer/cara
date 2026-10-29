#ifndef CARA_EVENT_MANAGER_H_INCLUDED
#define CARA_EVENT_MANAGER_H_INCLUDED

#include "cara/cara_status.h"
#include "cara/cara_event.h"
#include "cara_types.h"

cara_status_t cara_event_manager_create(cara_event_manager_t **p_event_manager);
void cara_event_manager_destroy(cara_event_manager_t *event_manger);

cara_status_t cara_event_reset(cara_event_manager_t *event_manager);

cara_status_t cara_event_insert(
	cara_event_manager_t *event_manager,
	cara_event_t event
);

cara_status_t cara_event_process(
	cara_event_manager_t *event_manager,
	cara_app_t *app
);

#endif // !CARA_EVENT_MANAGER_H_INCLUDED
