/* tain_tai_unpack.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>

#include "uchar.h"
#include "tain.h"


struct tain *
tain_tai_unpack(struct tain *t, const uchar_t *buf)
{
  uint64_t x;

  x = (uchar_t) buf[0];
  x <<= 8; x += (uchar_t) buf[1];
  x <<= 8; x += (uchar_t) buf[2];
  x <<= 8; x += (uchar_t) buf[3];
  x <<= 8; x += (uchar_t) buf[4];
  x <<= 8; x += (uchar_t) buf[5];
  x <<= 8; x += (uchar_t) buf[6];
  x <<= 8; x += (uchar_t) buf[7];
  t->sec = x;

  return t;
}


/* eof: tain_tai_unpack.c */
