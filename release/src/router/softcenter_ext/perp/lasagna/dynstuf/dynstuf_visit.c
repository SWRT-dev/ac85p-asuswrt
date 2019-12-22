/* dynstuf_visit.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void
dynstuf_visit(struct dynstuf *S, void (*visitor)(void *data, void *xtra), void *xtra)
{
    void    **stuf = S->stuf;
    size_t    items = S->items;
    size_t    i;

    for(i = 0; i < items; ++i){
        visitor(stuf[i], xtra);
    }

    return;
}


/* eof dynstuf_visit.c */
