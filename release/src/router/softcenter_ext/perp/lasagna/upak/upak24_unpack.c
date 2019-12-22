/* upak24_unpack.c
** upak, portable storage for unsigned integers
** wcm, 2010.12.09 - 2010.12.09
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uint32_t
upak24_unpack(const uchar_t b[3])
{
  uint32_t  u = 0;

  u = b[2];
  u <<= 8;
  u += b[1];
  u <<= 8;
  u += b[0];

  return u;
}

/* eof (upak24_unpack.c) */
