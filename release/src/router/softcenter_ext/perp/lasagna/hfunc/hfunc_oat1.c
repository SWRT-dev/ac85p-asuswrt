/* hfunc_oat1.c
** wcm, 2010.06.04 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* oat1:
**   "one-at-a-time"
**   credited to Bob Jenkins
**   as enumerated by Julienne Walker, "Eternally Confuzzled"
*/

/* hash function: */
uint32_t
hfunc_oat1(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h += *key;
      h += (h << 10);
      h += (h >> 6);
      ++key, --klen; 
  }

  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);

  return h;
}

/* eof (hfunc_oat1.c) */
