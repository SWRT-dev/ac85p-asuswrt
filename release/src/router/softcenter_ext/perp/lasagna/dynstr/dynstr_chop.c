/* dynstr_chop.c
** wcm, 2010.06.15 - 2010.06.15
** ===
*/

#include "dynstr.h"

void
dynstr_chop(dynstr_t *S)
{
    if((S->n > 0) && (S->s[S->n - 1] == '\n')){
        S->s[S->n - 1] = '\0';
        --S->n;
    }

    return; 
}

/* eof: dynstr_chop.c */
