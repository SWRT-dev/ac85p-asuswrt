/* dynstr_putn.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "buf.h"
#include "cstr.h"
#include "dynstr.h"


/* append lesser of n or strlen(str) chars from str to S */
int
dynstr_putn(dynstr_t *S, const char *str, size_t n)
{
    size_t grow = cstr_len(str);

    if(n < grow)
        grow = n;

    if(dynstr_grow(S, grow) == -1){
        /* allocation failure: */
        return -1;
    }
    buf_copy(&S->s[S->n], str, grow);
    S->n += grow;
    S->s[S->n] = '\0'; 

    return 0; 
}

/* eof: dynstr_putn.c */
