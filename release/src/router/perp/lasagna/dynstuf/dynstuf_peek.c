/* dynstuf_peek.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void *
dynstuf_peek(struct dynstuf *S)
{
    return ((S->items > 0) ? S->stuf[S->items - 1] : NULL);
}


/* eof dynstuf_peek.c */
