/* dynstr_put.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


int
dynstr_put(dynstr_t *to, dynstr_t *from)
{
    if(dynstr_grow(to, from->n) == -1){
        return -1;
    }

    to->n += cstr_copy(&to->s[to->n], from->s);
    return 0;
}

/* eof: dynstr_put.c */
