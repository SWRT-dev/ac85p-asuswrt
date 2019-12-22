/* upak24_pack.c
** upak, portable storage for unsigned integers
** wcm, 2010.12.09 - 2010.12.09
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uchar_t *
upak24_pack(uchar_t b[3], uint32_t u)
{
  b[0] = u & 255;
  u >>= 8;
  b[1] = u & 255;
  u >>= 8;
  b[2] = u & 255;

  return b;
}


/* eof (upak24_pack.c) */
