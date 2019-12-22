/* hfunc_rsuh.c
** wcm, 2010.05.27 - 2010.06.04
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* rsuh()
** universal hash function
** derived from:
**   Robert Sedgewick, Algorithms in C, 3rd Edition
**   p579, (Program 14.2)
**
** uses pseudo-random multiplier sequence
*/ 

/* hash function: */
uint32_t
hfunc_rsuh(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;
  uint32_t   a = 31415U, b = 27183U;

  while(klen){
      h = (h * a) + *key;
      a *= b;
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_rsuh.c) */
