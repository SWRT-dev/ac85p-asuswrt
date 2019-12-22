/* upak48_pack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2012.01.23
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uchar_t *
upak48_pack(uchar_t b[6], uint64_t u)
{
  b[0] = u & 255;
  u >>= 8;
  b[1] = u & 255;
  u >>= 8;
  b[2] = u & 255;
  u >>= 8;
  b[3] = u & 255;
  u >>= 8;
  b[4] = u & 255;
  b[5] = u >> 8;

  return b;
}

/* eof (upak48_pack.c) */
