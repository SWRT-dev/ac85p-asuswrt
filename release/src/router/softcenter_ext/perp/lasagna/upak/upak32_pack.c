/* upak32_pack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.06
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uchar_t *
upak32_pack(uchar_t b[4], uint32_t u)
{
  b[0] = u & 255;
  u >>= 8;
  b[1] = u & 255;
  u >>= 8;
  b[2] = u & 255;
  b[3] = u >> 8;

  return b;
}


/* eof (upak32_pack.c) */
