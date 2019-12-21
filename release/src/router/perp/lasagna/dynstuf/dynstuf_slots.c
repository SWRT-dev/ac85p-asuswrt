/* dynstuf_slots.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


size_t
dynstuf_slots(struct dynstuf *S)
{
    return S->slots;
}


/* eof dynstuf_slots.c */
