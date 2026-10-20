#ifndef CARA_STATUS_H_INCLUDED
#define CARA_STATUS_H_INCLUDED

typedef enum cara_status_t cara_status_t;

enum cara_status_t {
	CS_OK = 0,
	CS_INVALID_ARGUMENT,
	CS_OUT_OF_MEMORY,
	CS_PLATFORM_ERROR,
	CS_APP_ERROR,
	CS_UNKNOWN_ERROR
};

#endif // !CARA_STATUS_H_INCLUDED
