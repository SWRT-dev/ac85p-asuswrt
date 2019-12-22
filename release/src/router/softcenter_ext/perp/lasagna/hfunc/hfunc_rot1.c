/* hfunc_rot1.c
** wcm, 2010.05.27 - 2010.06.08
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* rot1()
** rotating hash
** attributed to Donald E. Knuth, The Art of Programming
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
hfunc_rot1(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 5) ^ (h >> 27)) ^ *key;
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_rot1.c) */
