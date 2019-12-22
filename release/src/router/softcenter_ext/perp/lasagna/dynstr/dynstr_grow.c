/* dynstr_grow.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


/* consider "grow" as for strlen, _not_ inclusive of nul */
int
dynstr_grow(dynstr_t *S, size_t grow)
{
    size_t  have = S->k;
    size_t  need = S->n + grow + 1;

    if(have < need){
        return dynstr_need(S, need);
    }

    return 0; 
}

/* eof: dynstr_grow.c */
