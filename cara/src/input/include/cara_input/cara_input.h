#ifndef CARA_INPUT_H_INCLUDED
#define CARA_INPUT_H_INCLUDED

#include <stdbool.h>

#include "cara_input_type.h"
#include "cara/cara_status.h"
#include "cara/cara_key.h"
#include "cara_internal/cara_type.h"

cara_status_t cara_input_create(cara_t *cara, cara_input_t **p_input);
void cara_input_destroy(cara_input_t *input);

void cara_input_clear_presses(cara_input_t *input);
void cara_input_key_down_event(cara_input_t *input, cara_key_t key);
void cara_input_key_up_event(cara_input_t *input, cara_key_t key);

bool cara_input_key_down(cara_input_t *input, cara_key_t key);
bool cara_input_key_pressed(cara_input_t *input, cara_key_t key);
bool cara_input_key_released(cara_input_t *input, cara_key_t key);

#endif // !CARA_INPUT_H_INCLUDED
