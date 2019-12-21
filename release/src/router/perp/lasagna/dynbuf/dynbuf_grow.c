/* dynbuf_grow.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stddef.h>
#include "dynbuf.h"


int
dynbuf_grow(struct dynbuf *d, size_t add)
{
    if(d->buf)
        add += d->p;

    return dynbuf_need(d, add);
}

/* eof: dynbuf_grow.c */
