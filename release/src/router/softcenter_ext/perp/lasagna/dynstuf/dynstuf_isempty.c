/* dynstuf_isempty.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


size_t
dynstuf_isempty(struct dynstuf *S)
{
    return (S->items == 0);
}


/* eof dynstuf_isempty.c */
