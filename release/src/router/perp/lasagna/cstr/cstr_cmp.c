/* cstr_cmp.c
** wcm, 2004.04.19 - 2009.11.09
** ===
*/
#include "cstr.h"

int cstr_cmp(const char *s1,  const char *s2)
{
  char c;

  for (;;) {
    c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
  }
  return ((int)(unsigned int)(unsigned char) c)
       - ((int)(unsigned int)(unsigned char) *s2);
}


/* eof (cstr_cmp.c) */
