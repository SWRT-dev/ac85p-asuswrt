/* dynstuf.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


int
dynstuf_grow(struct dynstuf *S, size_t add)
{
    size_t  have = S->slots;
    size_t  need = S->items + add;

    if(need < S->items){
        /* error: overflow on addition */
        return -1;
    }

    if((S->stuf == NULL) || (have < need)){
        void    **stuf =  NULL;
        size_t    check_need = need << 1;

        if(check_need < need){
            /* error: overflow on addition */
            return -1;
        }
        need = check_need;

        check_need = need * sizeof(void *);
        if(check_need < need){
            /* error: overflow on multiplication */
            return -1;
        }

        stuf = (void **)realloc(S->stuf, check_need);
        if(stuf != NULL){
            S->stuf = stuf;
            S->slots = need;
        } else {
            /* error: allocation failure */
            return -1;
        }
    }

    /* else: enough slots already: */
    /* no error: */
    return 0;
}


/* eof dynstuf_grow.c */
