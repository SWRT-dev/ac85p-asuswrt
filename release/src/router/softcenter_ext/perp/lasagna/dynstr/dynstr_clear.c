/* dynstr_clear.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"

void
dynstr_clear(dynstr_t *S)
{
    S->n = 0;
    if(S->k > 0) S->s[0] = '\0';

    return;
}

/* eof: dynstr_clear.c */
