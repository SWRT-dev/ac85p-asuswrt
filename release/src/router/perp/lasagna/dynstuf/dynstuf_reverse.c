/* dynstuf_reverse.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


/* DYNSTUF_SWAP() macro:
**   stuf is S->stuf,
**   t is supplied temporary storage of type void *,
**   A, B are indices in stuf[]
*/
#define DYNSTUF_SWAP(stuf,t,A,B) {\
    (t) = stuf[(A)]; stuf[(A)] = stuf[(B)]; stuf[(B)] = (t); }
    
void
dynstuf_reverse(struct dynstuf *S)
{
    void    **stuf = S->stuf;
    size_t    ix0, ixN;
    void     *tmp;

    if(S->items < 2)
        return;

    /* else: */
    for(ix0 = 0, ixN = S->items - 1; ix0 < ixN; ++ix0, --ixN){
        DYNSTUF_SWAP(stuf, tmp, ix0, ixN);
    }

    return; 
}


/* eof dynstuf_reverse.c */
