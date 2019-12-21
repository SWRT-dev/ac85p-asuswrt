/* dynbuf_freebuf.c
** wcm, 2004.05.01 - 2012.07.25
** ===
*/
#include <stdlib.h>
#include "dynbuf.h"

/* release internal buffer only: */

void
dynbuf_freebuf(struct dynbuf *d)
{
    if(d){
        if(d->buf) {
            free(d->buf);
            d->buf = NULL;
        }
        d->n = 0;
        d->p = 0;
    }
    return;
}

/* eof: dynbuf_freebuf.c */
