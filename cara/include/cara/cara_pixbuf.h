#ifndef CARA_PIXBUF_H_INCLUDED
#define CARA_PIXBUF_H_INCLUDED

#include <stdint.h>

typedef struct cara_pixbuf_t cara_pixbuf_t;

struct cara_pixbuf_t {
    const uint32_t *pixels;

    uint32_t width;
    uint32_t height;
    // bytes per row
    uint32_t pitch;
};

#endif // !CARA_PIXBUF_H_INCLUDED
