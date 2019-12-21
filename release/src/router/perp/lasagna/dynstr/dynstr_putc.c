/* dynstr_putc.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"


int
dynstr_putc(dynstr_t *S, char c)
{
    if(dynstr_grow(S, 1) == -1){
        /* allocation failure: */
        return -1;
    }
    S->s[S->n] = c;
    ++S->n;
    S->s[S->n] = '\0';

    return 0; 
}

/* eof: dynstr_putc.c */
