/* upak16_pack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.06
** ===
*/
#include <stdint.h>

#include "uchar.h"
#include "upak.h"


uchar_t *
upak16_pack(uchar_t b[2], uint16_t u)
{
  b[0] = u & 255;
  b[1] = u >> 8;

  return b;
}


/* eof (upak16_pack.c) */
