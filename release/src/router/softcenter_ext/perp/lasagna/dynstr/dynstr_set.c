/* dynstr_set.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


void
dynstr_set(dynstr_t *S, char *str)
{
    S->s = str;
    S->n = cstr_len(str);
    S->k = S->n + 1;

    return;
}


/* eof: dynstr_set.c */
