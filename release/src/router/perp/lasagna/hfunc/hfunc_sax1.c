/* hfunc_sax1.c
** wcm, 2010.06.04 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* sax1:
**   "shift/add/xor"
**   as enumerated by Julienne Walker, "Eternally Confuzzled"
**   (originally?)
*/

/* hash function: */
uint32_t
hfunc_sax1(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h ^= (h << 5) + (h >> 2)  + *key;
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_sax1.c) */
