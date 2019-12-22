/* dynstr_puts.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


int
dynstr_puts(dynstr_t *S, const char *str)
{
    if(dynstr_grow(S, cstr_len(str)) == -1){
        /* allocation failure: */
        return -1;
    }
    S->n += cstr_copy(&S->s[S->n], str);

    return 0; 
}

/* eof: dynstr_puts.c */
