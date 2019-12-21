/* cstr_len_small.c
** wcm, 2004.04.19 - 2010.01.18
** ===
*/

#include <stddef.h>
#include "cstr.h"

size_t
cstr_len(const char *s)
{
  const char *p = s;

  for (;;) {
    if (!*p) break; ++p;
  }

  return (p - s);
}

/* eof (cstr_len_small.c) */ 
