#ifndef CARA_KEY_H_INCLUDED
#define CARA_KEY_H_INCLUDED

typedef enum cara_key_t cara_key_t;

enum cara_key_t {
	// Letter keys
	KEY_A, KEY_B,
	KEY_C, KEY_D,
	KEY_E, KEY_F,
	KEY_G, KEY_H,
	KEY_I, KEY_J,
	KEY_K, KEY_L,
	KEY_M, KEY_N,
	KEY_O, KEY_P,
	KEY_Q, KEY_R,
	KEY_S, KEY_T,
	KEY_U, KEY_V,
	KEY_W, KEY_X,
	KEY_Y, KEY_Z,
	// Number keys
	KEY_0, KEY_1,
	KEY_2, KEY_3,
	KEY_4, KEY_5,
	KEY_6, KEY_7,
	KEY_8, KEY_9,
	// Arrow keys
	KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
	// Special keys
	KEY_PLUS, KEY_MINUS,
	KEY_SPACE,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_CTRL,
	KEY_ALT,
	KEY_SHIFT,
	KEY_TAB,
	KEY_BACK,

	KEY_UNKNOWN,
	KEY_COUNT
};

#endif // !CARA_KEY_H_INCLUDED
