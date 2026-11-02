#ifndef CARA_CORE_H_INCLUDED
#define CARA_CORE_H_INCLUDED

#include "cara_core_type.h"

#include "cara/cara_status.h"
#include "cara/cara_app.h"
#include "cara_internal/cara_type.h"

/**
 * @brief			Creates and initializes the core subsystem.
 * @param cara		Pointer to the owning runtime instance.
 * @param p_core	Receives the created core instance.
 * @return			CARA_STATUS_OK on success.
 * @note			On failure, *p_core is set to NULL.
 */
cara_status_t cara_core_create(cara_t *cara, cara_core_t **p_core);

/**
 * @brief			Destroys a core instance
 * @param core		Core instance to destroy.
 */
void cara_core_destroy(cara_core_t *core);


/**
 * @brief			Requests termination of the main loop.
 * @param core		Core instance receving the quit request.
 */
void cara_core_request_quit(cara_core_t *core);

/**
 * @brief			Starts the application and executes the main loop.
 * @param core		Core instance.
 * @param app		Application whose callbacks are executed.
 * @return			CARA_STATUS_OK if execution completes successfully.
 * @note			The application shutdown callback is called after the
 *					main loop, provided application initialization succeeded.
 */
cara_status_t cara_core_run(cara_core_t *core, cara_app_t *app);

#endif // !CARA_CORE_H_INCLUDED
