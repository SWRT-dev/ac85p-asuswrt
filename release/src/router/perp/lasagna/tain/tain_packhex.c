/* tain_packhex.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2009.08.04 - 2009.08.04
** ===
*/

#include "uchar.h"
#include "tain.h"

static const char hex[16] = "0123456789abcdef";

char *
tain_packhex(char *s, const tain_t *t)
{
  uchar_t  tpack[TAIN_PACK_SIZE];
  int      i;

  tain_pack(tpack, t);
  for(i = 0; i < TAIN_PACK_SIZE; ++i){
      s[i * 2] = hex[(tpack[i] >> 4) & 15];
      s[(i * 2) + 1] = hex[tpack[i] & 15];
  }
  s[24] = '\0';

  return s;
}
  

/* EOF (tain_packhex.c) */
