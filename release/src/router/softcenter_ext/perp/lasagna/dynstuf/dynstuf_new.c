/* dynstuf_new.c
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2008.10.16
** ===
*/
#include <stdlib.h>

#include "dynstuf.h"


struct dynstuf *
dynstuf_new(void)
{
    return dynstuf_init(NULL);
}


/* eof dynstuf_new.c */
