/* dynstuf_free.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


void
dynstuf_free(struct dynstuf *S, void (*free_data)(void *data))
{
    void     **stuf = S->stuf;
    size_t     i = 0;

    if(free_data != NULL){
        for(i = 0; i < S->items; ++i){
            free_data(stuf[i]);
        }
    }

    free(stuf);
    free(S);
    return;
}


/* eof dynstuf_free.c */
