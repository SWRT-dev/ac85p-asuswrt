/* tain_tai_pack.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>

#include "uchar.h"
#include "tain.h"


uchar_t *
tain_tai_pack(uchar_t *buf, const struct tain *t)
{
  uint64_t x = t->sec;

  /* 8 byte TAI64 label: */
  buf[7] = x & 255; x >>= 8;
  buf[6] = x & 255; x >>= 8;
  buf[5] = x & 255; x >>= 8;
  buf[4] = x & 255; x >>= 8;
  buf[3] = x & 255; x >>= 8;
  buf[2] = x & 255; x >>= 8;
  buf[1] = x & 255; x >>= 8;
  buf[0] = x;

  return buf;
}


/* eof: tain_tai_pack.c */
