/* buf_cmp.c
** wcm, 2004.04.20 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

int buf_cmp(const void *b1, const void *b2, size_t n)
{
  const uchar_t  *p1 = b1;
  const uchar_t  *p2 = b2;

  for (;;) {
    if (!n) return (int)0; if (*p1 != *p2) break; ++p1; ++p2; --n;
    if (!n) return (int)0; if (*p1 != *p2) break; ++p1; ++p2; --n;
    if (!n) return (int)0; if (*p1 != *p2) break; ++p1; ++p2; --n;
    if (!n) return (int)0; if (*p1 != *p2) break; ++p1; ++p2; --n;
  }

  return ((int)(unsigned int) *p1) - ((int)(unsigned int) *p2);
}

/* eof: buf_cmp.c */
