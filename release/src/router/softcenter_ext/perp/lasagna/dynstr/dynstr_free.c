/* dynstr_free.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"

void
dynstr_free(dynstr_t *S)
{
    if(S != NULL){
        if(S->s != NULL) free(S->s);
        free(S);
    }
    return;
}

/* eof: dynstr_free.c */
