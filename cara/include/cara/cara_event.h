#ifndef CARA_EVENT_H_INCLUDED
#define CARA_EVENT_H_INCLUDED

typedef struct cara_event_t cara_event_t;
typedef enum cara_event_type_t cara_event_type_t;

enum cara_event_type_t
{
	CARA_CLOSE_REQUEST,
	CARA_RESIZED
};

struct cara_event_t {
	cara_event_type_t type;

	union {
		struct {
			uint32_t new_width;
			uint32_t new_height;
		} resize;
	};
};

#endif // !CARA_EVENT_H_INCLUDED
