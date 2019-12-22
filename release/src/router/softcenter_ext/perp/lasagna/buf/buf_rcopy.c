/* buf_rcopy.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

void *
buf_rcopy(void *to, const void *from,  size_t n)
{
  uchar_t        *p1 = to;
  const uchar_t  *p2 = from;

  p1 += n;
  p2 += n;
  for (;;)
  {
    if (!n) break; *--p1 = *--p2; --n;
    if (!n) break; *--p1 = *--p2; --n;
    if (!n) break; *--p1 = *--p2; --n;
    if (!n) break; *--p1 = *--p2; --n;
  }

  return to;
}

/* eof: buf_rcopy.c */
