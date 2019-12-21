/* cstr_contains.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include "cstr.h"

int cstr_contains(const char *s1, const char *s2)
{
  char c;

  for (;;) {
    c = *s2++; if (!c) break; if (c != *s1++) return 0;
    c = *s2++; if (!c) break; if (c != *s1++) return 0;
    c = *s2++; if (!c) break; if (c != *s1++) return 0;
    c = *s2++; if (!c) break; if (c != *s1++) return 0;
  }

  /* s2 contained in s1: */
  return 1;
}

/* eof (cstr_contains.c) */
