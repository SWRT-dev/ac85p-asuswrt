/* upak32_unpack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.06
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uint32_t
upak32_unpack(const uchar_t b[4])
{
  uint32_t  u = 0;

  u = b[3];
  u <<= 8;
  u += b[2];
  u <<= 8;
  u += b[1];
  u <<= 8;
  u += b[0];

  return u;
}

/* eof (upak32_unpack.c) */
