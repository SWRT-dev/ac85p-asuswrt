/* dynbuf.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stdlib.h>
#include "dynbuf.h"

/* accessors: */

void *
dynbuf_buf(struct dynbuf *d)
{
    return d->buf;
}

size_t
dynbuf_len(struct dynbuf *d)
{
    return d->p;
}

/* eof: dynbuf.c */
