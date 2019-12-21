/* hfunc_pjw1.c
** wcm, 2010.05.27 - 2010.06.07
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* pjw1()
** attributed to P.J. Weinberger
** numerous adaptations
** see: Compilers:..., Aho/Sethi/Ullman
*/ 

/* hash function: */
uint32_t
hfunc_pjw1(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;
  uint32_t   g;

  while(klen){
      h = (h << 4) + *key;
      g = h & 0xf0000000u;
      if(g != 0){
          h ^= (g >> 24) ^ g;
      }
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_pjw1.c) */
