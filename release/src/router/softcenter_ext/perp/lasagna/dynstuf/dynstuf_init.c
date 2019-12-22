/* dynstuf_init.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.24
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


/* local enumerated constants: */
enum {
    DYNSTUF_INIT = 1
};


struct dynstuf *
dynstuf_init(struct dynstuf *S_)
{
    struct dynstuf *S = S_;
    int             new = 0;

    if(S == NULL){
        S = (struct dynstuf *)malloc(sizeof(struct dynstuf));
        if(S == NULL)
            return NULL;
        /* else: */
        new = 1;
    }

    S->stuf = NULL;
    S->slots = 0;
    S->items = 0;
    if(dynstuf_grow(S, DYNSTUF_INIT) != 0){
        if(new) free(S);
        return NULL;
    }

    return S;
}


/* eof dynstuf_init.c */
