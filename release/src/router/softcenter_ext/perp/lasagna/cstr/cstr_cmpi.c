/* cstr_cmpi.c
** wcm, 2004.04.19 - 2010.01.07
** ===
*/
#include <ctype.h> /* tolower() */

#include "cstr.h"

int cstr_cmpi(const char *s1, const char *s2)
{
  int x;

  for(;;){
      x = tolower((int)*s1); if(x != tolower((int)*s2)) break; if (!x) break; ++s1; ++s2;
      x = tolower((int)*s1); if(x != tolower((int)*s2)) break; if (!x) break; ++s1; ++s2;
      x = tolower((int)*s1); if(x != tolower((int)*s2)) break; if (!x) break; ++s1; ++s2;
      x = tolower((int)*s1); if(x != tolower((int)*s2)) break; if (!x) break; ++s1; ++s2;
  }

  return (x - tolower((int)*s2));
}

/* eof (cstr_cmpi.c) */
