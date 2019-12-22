/* dynstuf_push.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


int
dynstuf_push(struct dynstuf *S, void *data)
{
    if(dynstuf_grow(S, 1) == 0){
        S->stuf[S->items] = data;
        ++S->items;
        return 0;
    }

    /* else: allocation failure in dynstuf_grow() */
    return -1;
}


/* eof dynstuf_push.c */
