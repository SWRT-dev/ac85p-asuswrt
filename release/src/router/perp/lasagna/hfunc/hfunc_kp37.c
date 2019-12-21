/* hfunc_kp37.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* kp37
** kernighan/pike, The Practice of Programming, p57
** equivalent to ghfa(key, klen, 0, 37)
**   initializer:   0
**   multiplier:   37
**
** this version uses bit shift/add operation for multiplier
** ie:  (h * 37) == ((h << 5) + (h << 2) + h)
*/

uint32_t
hfunc_kp37(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 5) + (h << 2) + h) + *key;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_kp37.c) */
