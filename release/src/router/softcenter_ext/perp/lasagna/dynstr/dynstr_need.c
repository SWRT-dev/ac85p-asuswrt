/* dynstr_need.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


/* consider "need" as for strlen, _not_ inclusive of nul */
int
dynstr_need(dynstr_t *S, size_t need)
{
    size_t  have = S->k;

    /* accomodate need for terminating-nul: */
    ++need;
    if((S->s == NULL) || (have < need)){
        char *t = (char *)realloc(S->s, need * sizeof(char));
        if(t == NULL) return -1;
        S->s = t;
        S->k = need;
    }

    return 0; 
}

/* eof: dynstr_need.c */
