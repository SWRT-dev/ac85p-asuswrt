/* cstr_copy.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include <stddef.h>
#include "cstr.h"

size_t
cstr_copy(char *to, const char *from)
{
  char *to0 = to;

  for (;;) {
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
    if (!(*to = *from)) break; ++to; ++from;
  }

  return (to - to0);
}


/* eof: cstr_copy.c */
