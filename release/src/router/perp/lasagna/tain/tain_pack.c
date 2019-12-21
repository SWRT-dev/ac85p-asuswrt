/* tain_pack.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>

#include "uchar.h"
#include "tain.h"

uchar_t *
tain_pack(uchar_t *buf, const struct tain *t)
{
  uint32_t x;

  /* first 8 bytes are TAI64 label seconds: */
  tain_tai_pack(buf, t);
  buf += 8;

  /* next 4 bytes (for TAI64N label nanoseconds): */
  x = t->nsec;
  buf[3] = x & 255; x >>= 8;
  buf[2] = x & 255; x >>= 8;
  buf[1] = x & 255; x >>= 8;
  buf[0] = x;

  return buf;
}


/* EOF (tain_pack.c) */
