/* buf_ndx.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

size_t
buf_ndx(const void *buf,  size_t n, int c)
{
   const uchar_t  *p = buf;
   uchar_t         ch = (uchar_t)c;

  for (;;) {
    if (!n) break; if (*p == ch) break; ++p; --n;
    if (!n) break; if (*p == ch) break; ++p; --n;
    if (!n) break; if (*p == ch) break; ++p; --n;
    if (!n) break; if (*p == ch) break; ++p; --n;
  }

  return (size_t)((uchar_t *)p - (uchar_t *)buf);
}

/* eof: buf_ndx.c */
