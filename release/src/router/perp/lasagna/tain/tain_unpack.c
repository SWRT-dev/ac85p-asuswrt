/* tain_unpack.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>

#include "uchar.h"
#include "tain.h"


struct tain *
tain_unpack(struct tain *t, const uchar_t *buf)
{
  uint32_t x;

  /* first 8 bytes are TAI64 label: */
  tain_tai_unpack(t, buf);
  buf += 8;

  /* next 4 bytes are nanoseconds: */
  x = (uchar_t) buf[0];
  x <<= 8; x += (uchar_t) buf[1];
  x <<= 8; x += (uchar_t) buf[2];
  x <<= 8; x += (uchar_t) buf[3];
  t->nsec = x;

  return t;
}


/* EOF (tain_unpack.c) */
