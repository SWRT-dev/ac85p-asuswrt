/* hfunc_rotm.c
** wcm, 2010.06.09 - 2010.06.09
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* rotm()
** rotating hash, modified
** start with rot1()
** reorder combine and mix operations
** append post-processing as for fnvm()
**
** mix operation rotates in 5 bits of previous key
** combine operation xor's next byte of key
**
** this implementation arbitrarily initializes h:
**   phi * 2^32 =   2654435769
**   closest prime: 2654435761  (hex: 0x9e3779b1)
*/ 

/* hash function: */
uint32_t
hfunc_rotm(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h ^= *key;
      h = ((h << 5) ^ (h >> 27));
      ++key; --klen;
  }

  /* post-processing: */
  h += h << 13;
  h ^= h >> 7;
  h += h << 3;
  h ^= h >> 17;
  h += h << 5;

  return h;
}

/* eof (hfunc_rotm.c) */
