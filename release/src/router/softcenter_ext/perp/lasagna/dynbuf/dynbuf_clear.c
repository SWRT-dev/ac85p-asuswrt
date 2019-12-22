/* dynbuf_clear.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include "dynbuf.h"

void
dynbuf_clear(struct dynbuf *d)
{
    d->p = 0;
    return;
}

/* eof: dynbuf_clear.c */
