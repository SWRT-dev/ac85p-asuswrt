/* dynstuf_set.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void *
dynstuf_set(struct dynstuf *S, size_t slot, void *data)
{
    if(slot < S->items){
        return (S->stuf[slot] = data);
    }

    /* else: */
    return NULL;
}


/* eof dynstuf_set.c */
