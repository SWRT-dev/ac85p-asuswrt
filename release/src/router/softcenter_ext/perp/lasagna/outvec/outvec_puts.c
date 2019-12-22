/* outvec_puts.c
** output using writev()
** wcm, 2010.11.27 - 2010.12.01
** ===
*/

#include "cstr.h"

#include "outvec.h"


int
outvec_puts(struct outvec *vec, const char *s)
{
    return outvec_put(vec, s, cstr_len(s));
}


/* eof: outvec_puts.c */
