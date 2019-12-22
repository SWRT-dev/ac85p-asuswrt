/* dynstuf_stuf.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void **
dynstuf_stuf(struct dynstuf *S)
{
    return S->stuf;
}


/* eof dynstuf_stuf.c */
