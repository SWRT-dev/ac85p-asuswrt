/* dynbuf_free.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stdlib.h>
#include "dynbuf.h"


void
dynbuf_free(struct dynbuf *d)
{
    if(d){
        if(d->buf) free(d->buf);
        free(d);
    }
    return;
}

/* eof: dynbuf_free.c */
