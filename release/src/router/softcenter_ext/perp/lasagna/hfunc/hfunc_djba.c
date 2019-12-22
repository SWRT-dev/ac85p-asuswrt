/* hfunc_djba.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* djba
** daniel bernstein, w/ additive mixing of each key byte
** equivalent to ghfa(key, klen, 5381, 33)
**   initializer:  5381
**   multiplier:     33
**
** this version uses bit shift/add operation for multiplier
** ie:  (h * 33) == ((h << 5) + h)
*/

uint32_t
hfunc_djba(const uchar_t *key, size_t klen)
{
  uint32_t   h = 5381;

  while(klen){
      h = ((h << 5) + h) + *key;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_djba.c) */
