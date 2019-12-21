/* dynstuf_pop.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void *
dynstuf_pop(struct dynstuf *S)
{
    void *data = NULL;

    if(S->items > 0){
        --S->items;
        data = S->stuf[S->items];
    }

    return data;
}


/* eof dynstuf_pop.c */
