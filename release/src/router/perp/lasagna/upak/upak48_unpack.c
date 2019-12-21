/* upak48_unpack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2012.07.23
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uint64_t
upak48_unpack(const uchar_t b[6])
{
  uint64_t  u = 0L;

  u = b[5];
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

/* eof (upak48_unpack.c) */
