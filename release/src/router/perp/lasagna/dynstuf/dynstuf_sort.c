/* dynstuf_sort.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"

#if 0
/* DYNSTUF_SWAP() macro:
**   stuf is S->stuf,
**   t is supplied temporary storage of type void *,
**   A, B are indices in stuf[]
*/
#define DYNSTUF_SWAP(stuf,t,A,B) {\
    (t) = stuf[(A)]; stuf[(A)] = stuf[(B)]; stuf[(B)] = (t); }
#endif

void
dynstuf_sort(struct dynstuf *S, int(*cmp)(const void *a, const void *b))
{
    /* using stdlibc qsort: */
    qsort(S->stuf, S->items, sizeof(void *), cmp);
    return;
}


/* eof dynstuf_sort.c */
