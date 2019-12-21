/* cstr_len.c
** wcm, 2004.04.19 - 2009.08.03
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
    if (!*p) break; ++p;
    if (!*p) break; ++p;
    if (!*p) break; ++p;
  }

  return (p - s);
}

/* eof (cstr_len.c) */ 
