/* dynstuf_get.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void *
dynstuf_get(struct dynstuf *S, size_t slot)
{
    if(slot < S->items){
        return S->stuf[slot];
    }

    /* else: */
    return NULL;
}


/* eof dynstuf_get.c */
