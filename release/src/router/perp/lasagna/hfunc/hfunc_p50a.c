/* hfunc_p50a.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* p50a
** perl5, versions < 5.8
** equivalent to ghfa(key, klen, 0, 33)
**   initializer:   0
**   multiplier:   33
**
** this version uses bit shift/add operation for multiplier
** ie:  (h * 33) == ((h << 5) + h)
*/

uint32_t
hfunc_p50a(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 5) + h) + *key;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_p50a.c) */
