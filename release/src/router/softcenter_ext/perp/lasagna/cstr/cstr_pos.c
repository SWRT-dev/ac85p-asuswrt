/* cstr_pos.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include <stddef.h>
#include "cstr.h"

size_t
cstr_pos(const char *s, int c)
{
  const char  *p = s;
  char         ch = (char)c;

  for (;;) {
    if (!*p) break; if (*p == ch) break; ++p;
    if (!*p) break; if (*p == ch) break; ++p;
    if (!*p) break; if (*p == ch) break; ++p;
    if (!*p) break; if (*p == ch) break; ++p;
  }

  return p - s;
}


/* eof (cstr_pos.c) */
