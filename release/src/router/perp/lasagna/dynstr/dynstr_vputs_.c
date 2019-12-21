/* dynstr_vputs_.c
** wcm, 2009.09.10 - 2009.09.10
** ===
*/
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cstr.h"
#include "dynstr.h"


int
dynstr_vputs_(dynstr_t *S, const char *s0, ...)
{
    va_list      args;
    const char  *s = s0;
    int          e = 0;

    va_start(args, s0);
    do{
        e = dynstr_puts(S, s);
        if(e == -1) break;
        s = va_arg(args, const char *);
    }while(s != NULL);
    va_end(args);

    return e;
}

/* eof: dynstr_vputs_.c */
