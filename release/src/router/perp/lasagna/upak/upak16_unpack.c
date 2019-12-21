/* upak16_unpack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.06
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"

uint16_t
upak16_unpack(const uchar_t b[2])
{
  uint16_t  u = 0;

  u = b[1];
  u <<= 8;
  u += b[0];

  return u;
}

/* eof (upak16_unpack.c) */
