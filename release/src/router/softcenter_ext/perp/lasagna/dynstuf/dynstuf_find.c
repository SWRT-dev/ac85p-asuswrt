/* dynstuf_find.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


size_t
dynstuf_find(
    struct dynstuf *S,
    size_t          slot,
    void           *key,
    int           (*cmp)(const void *key1, const void *key2)
)
{
    size_t  items = S->items;

    while(slot < items){
        if(cmp(S->stuf[slot], key) == 0){
            /* match found: */
            return slot;
        }
        ++slot;
    }
    
    /* match not found: */
    return items;
}


/* eof dynstuf_find.c */
