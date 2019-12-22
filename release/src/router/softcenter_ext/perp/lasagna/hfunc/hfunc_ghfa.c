/* hfunc_ghfa.c
** wcm, 2010.05.27 - 2010.06.04
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* hash function: */
uint32_t
hfunc_ghfa(const uchar_t *key, size_t klen, uint32_t init, uint32_t mult)
{
  uint32_t   h = init;

  while(klen){
      h = (h * mult) + *key;
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_ghfa.c) */
