/* buf_copy.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>

#include "uchar.h"
#include "buf.h"

void *
buf_copy(void *to, const void *from, size_t n)
{
   uchar_t       *p1 = to;
   const uchar_t *p2 = from;
  
  for (;;) {
    if (!n) break; *p1++ = *p2++; --n;
    if (!n) break; *p1++ = *p2++; --n;
    if (!n) break; *p1++ = *p2++; --n;
    if (!n) break; *p1++ = *p2++; --n;
  }

  return to;
}

/* eof: buf_copy.c */
