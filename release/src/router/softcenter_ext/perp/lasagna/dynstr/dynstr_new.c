/* dynstr_new.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stdlib.h>

#include "dynstr.h"

dynstr_t *
dynstr_new(void)
{
    dynstr_t  *S = NULL;
    char      *s = NULL;
    size_t     k = 0;

    S = (dynstr_t *)malloc(sizeof(dynstr_t));
    if(S == NULL){
        return NULL;
    }

    k = DYNSTR_INITSIZE + 1;
    s = (char *)malloc(k * sizeof(char));
    if(s == NULL){
        free(S);
        return NULL;
    }

    S->s = s;
    S->k = k;
    S->s[0] = '\0';
    S->n = 0;

    return S;
}

/* eof: dynstr_new.c */
