/* buf_rndx.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

size_t buf_rndx(const void *b,  size_t n, int c)
{
  const uchar_t  *p = b;
  const uchar_t  *pN = 0;
  uchar_t         ch = (uchar_t)c;

  for (;;) {
    if (!n) break; if (*p == ch) pN = p; ++p; --n;
    if (!n) break; if (*p == ch) pN = p; ++p; --n;
    if (!n) break; if (*p == ch) pN = p; ++p; --n;
    if (!n) break; if (*p == ch) pN = p; ++p; --n;
  }
  if (!pN) pN = p;

  return (size_t)((uchar_t *)pN - (uchar_t *)b);
}

/* eof: buf_rndx.c */
