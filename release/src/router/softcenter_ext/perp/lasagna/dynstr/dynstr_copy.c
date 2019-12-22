/* dynstr_copy.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


int
dynstr_copy(dynstr_t *to, dynstr_t *from)
{
    if(dynstr_need(to, from->n) == -1){
        /* allocation failure: */
        return -1;
    }
    to->n = cstr_copy(to->s, from->s);

    return 0; 
}

/* eof: dynstr_copy.c */
