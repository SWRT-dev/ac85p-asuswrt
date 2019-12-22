/* cstr_lcat.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/

#include <stddef.h>  /* size_t */
#include "cstr.h"

size_t
cstr_lcat(char *dest, const char *src, size_t dest_len)
{
  char        *to = dest;
  const char  *from = src;
  size_t       n = dest_len;
  size_t                n0 = 0;

  for(;;){
      if (!n || !(*to)) break; --n; ++to;
      if (!n || !(*to)) break; --n; ++to;
      if (!n || !(*to)) break; --n; ++to;
      if (!n || !(*to)) break; --n; ++to;
  }

  n0 = to - dest;    /* initial strlen(dest) */
  n = dest_len - n0; /* remaining in dest_len */

  if(n == 0)
      return (n0 + cstr_len(src));

  for (;;) {
      if(!(*from)) break; if(n != 1) {*to++ = *from; --n;} from++;
      if(!(*from)) break; if(n != 1) {*to++ = *from; --n;} from++;
      if(!(*from)) break; if(n != 1) {*to++ = *from; --n;} from++;
      if(!(*from)) break; if(n != 1) {*to++ = *from; --n;} from++;
  }
  *to = '\0';

  return (n0 + (from - src));
}

/* eof (cstr_lcat.c) */
