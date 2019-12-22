/* dynstr.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"

/* accessors: */

char *
dynstr_str(dynstr_t *S)
{
    return S->s;
}

size_t
dynstr_len(dynstr_t *S)
{
    return S->n;
}

/* eof: dynstr.c */
