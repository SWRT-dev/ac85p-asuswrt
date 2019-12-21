/* cstr_cat.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include <stddef.h>  /* size_t */
#include "cstr.h"


size_t
cstr_cat( char *to,  const char *from)
{
  const char  *to0 = to;

  for (;;) {
    if (!(*to)) break; ++to;
    if (!(*to)) break; ++to;
    if (!(*to)) break; ++to;
    if (!(*to)) break; ++to;
  }

  for (;;) {  
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
  }

  return (to - to0);
}


/* eof (cstr_cat.c) */
