/* hfunc_kr31.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* kr31
** kernighan/ritchie: k&rc/2e, p144
** equivalent to ghfa(key, klen, 0, 31)
**   initializer:   0
**   multiplier:   31
**
** this version uses bit shift/add operation for multiplier
** ie:  (h * 31) == ((h << 5) - h)
*/

uint32_t
hfunc_kr31(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 5) - h) + *key;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_kr31.c) */
