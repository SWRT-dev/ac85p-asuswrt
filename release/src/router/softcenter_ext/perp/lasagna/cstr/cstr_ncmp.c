/* cstr_ncmp.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include "uchar.h"
#include "cstr.h"


int
cstr_ncmp(const char *s1, const char *s2, size_t len)
{
  char c;

  for (;;) {
    if (!len--) return 0; c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    if (!len--) return 0; c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    if (!len--) return 0; c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
    if (!len--) return 0; c = *s1; if (c != *s2) break; if (!c) break; ++s1; ++s2;
  }

  return ((int)(unsigned int)(uchar_t) c)
       - ((int)(unsigned int)(uchar_t) *s2);
}

/* eof (cstr_ncmp.c) */
