#include <stdlib.h>
#include <string.h>

#include "cara_input/cara_input.h"

typedef struct keystate_t keystate_t;

struct keystate_t {
	bool is_down;
	bool was_pressed;
	bool was_released;
};

struct cara_input_t {
	cara_t *cara;
	keystate_t keystates[KEY_COUNT];
};

cara_status_t cara_input_create(cara_t *cara, cara_input_t **p_input)
{
	cara_input_t *input = calloc(1, sizeof(*input));
	if (!input) {
		return CARA_STATUS_OUT_OF_MEMORY;
	}

	input->cara = cara;
	memset(input->keystates, 0, KEY_COUNT * sizeof(*input->keystates));

	*p_input = input;

	return CARA_STATUS_OK;
}

void cara_input_destroy(cara_input_t *input)
{
	if (!input) {
		return;
	}

	free(input);
}

void cara_input_clear_presses(cara_input_t *input)
{
	for (int key = 0; key < KEY_COUNT; key++) {
		input->keystates[key].was_pressed = false;
		input->keystates[key].was_released = false;
	}
}

void cara_input_key_down_event(cara_input_t *input, cara_key_t key)
{
	if (!input->keystates[key].is_down) {
		input->keystates[key].was_pressed = true;
	}
	input->keystates[key].is_down = true;
}

void cara_input_key_up_event(cara_input_t *input, cara_key_t key)
{
	if (input->keystates[key].is_down) {
		input->keystates[key].was_released = true;
	}
	input->keystates[key].is_down = false;
}


bool cara_input_key_down(cara_input_t *input, cara_key_t key)
{
	return input->keystates[key].is_down;
}

bool cara_input_key_pressed(cara_input_t *input, cara_key_t key)
{
	return input->keystates[key].was_pressed;
}

bool cara_input_key_released(cara_input_t *input, cara_key_t key)
{
	return input->keystates[key].was_released;
}

