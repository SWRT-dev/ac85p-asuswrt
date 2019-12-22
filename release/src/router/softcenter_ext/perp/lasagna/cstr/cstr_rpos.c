/* cstr_rpos.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include <stddef.h>
#include "cstr.h"

size_t
cstr_rpos(const char *s, int c)
{
  const char  *p = s;
  char         ch = (char)c;
  const char  *t;

  t = 0;
  for (;;) {
    if (!*p) break; if (*p == ch) t = p; ++p;
    if (!*p) break; if (*p == ch) t = p; ++p;
    if (!*p) break; if (*p == ch) t = p; ++p;
    if (!*p) break; if (*p == ch) t = p; ++p;
  }
  if (!t) t = p;

  return t - s;
}


/* eof (cstr_pos.c) */
