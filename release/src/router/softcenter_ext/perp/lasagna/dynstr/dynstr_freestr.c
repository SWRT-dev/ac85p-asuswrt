/* dynstr_freestr.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"

void
dynstr_freestr(dynstr_t *S)
{
    if(S->s != NULL) free(S->s);

    S->s = NULL;
    S->k = 0;
    S->n = 0;

    return;
}

/* eof: dynstr_free.c */
