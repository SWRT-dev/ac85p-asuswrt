/* upak64_unpack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.06
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uint64_t
upak64_unpack(const uchar_t b[8])
{
  uint64_t  u = 0L;

  u = b[7];
  u <<= 8;
  u += b[6];
  u <<= 8;
  u += b[5];
  u <<= 8;
  u += b[4];
  u <<= 8;
  u += b[3];
  u <<= 8;
  u += b[2];
  u <<= 8;
  u += b[1];
  u <<= 8;
  u += b[0];

  return u;
}

/* eof (upak64_unpack.c) */
