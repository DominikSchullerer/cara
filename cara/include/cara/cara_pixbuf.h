#ifndef CARA_PIXBUF_H_INCLUDED
#define CARA_PIXBUF_H_INCLUDED

#include <stdint.h>

typedef struct cara_pixbuf_t cara_pixbuf_t;

struct cara_pixbuf_t {
    uint32_t *pixels;

    uint32_t width;
    uint32_t height;
};

#endif // !CARA_PIXBUF_H_INCLUDED
