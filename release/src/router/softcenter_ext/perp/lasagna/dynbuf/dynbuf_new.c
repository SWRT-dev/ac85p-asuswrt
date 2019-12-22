/* dynbuf_new.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stdlib.h>
#include "dynbuf.h"

struct dynbuf *
dynbuf_new()
{
    struct dynbuf *d = (struct dynbuf *)malloc(sizeof(struct dynbuf));

    if(d){
        d->buf = (void *)malloc(DYNBUF_INITSIZE);
        if(d->buf != NULL){
            d->n = DYNBUF_INITSIZE;
            d->p = 0;
        }
        else{
            free(d);
            d = NULL;
        }
    }

    return d;
} 

/* eof: dynbuf_new.c */
