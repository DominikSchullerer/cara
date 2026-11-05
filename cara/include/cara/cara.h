#ifndef CARA_H_INCLUDED
#define CARA_H_INCLUDED

#include <stdbool.h>

#include "cara/cara_config.h"
#include "cara/cara_app.h"
#include "cara/cara_status.h"
#include "cara/cara_key.h"

typedef struct cara_t cara_t;

cara_status_t cara_create(const cara_config_t *config, cara_t **p_cara);
void cara_destroy(cara_t *cara);

cara_status_t cara_run(cara_t *cara, cara_app_t *app);
void cara_request_quit(cara_t *cara);

bool cara_key_down(cara_t *cara, cara_key_t key);
bool cara_key_pressed(cara_t *cara, cara_key_t key);
bool cara_key_released(cara_t *cara, cara_key_t key);

#endif // !CARA_H_INCLUDED
