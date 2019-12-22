/* buf_fill.c
** wcm, 2004.04.20 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

void *
buf_fill(void *b, size_t n, int c)
{
  uchar_t *p = b;

  for (;;) {
      if (!n) break; *p++ = (uchar_t)c; --n;
      if (!n) break; *p++ = (uchar_t)c; --n;
      if (!n) break; *p++ = (uchar_t)c; --n;
      if (!n) break; *p++ = (uchar_t)c; --n;
  }

  return b;
}

/* eof: buf_fill.c */
