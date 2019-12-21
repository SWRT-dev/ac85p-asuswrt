/* dynstuf_replace.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void *
dynstuf_replace(struct dynstuf *S, size_t slot, void *newdata)
{
    void *data = NULL;

    if(slot < S->items){
        data = S->stuf[slot];
        S->stuf[slot] = newdata;
    }

    return data;
}


/* eof dynstuf_replace.c */
