/* hfunc_djbm.c
** wcm, 2010.06.09 - 2010.06.09
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* djbm
** djb hash, modified:
**   mix/combine operations reordered
**   post-processesing as in fnvm()
*/

uint32_t
hfunc_djbm(const uchar_t *key, size_t klen)
{
  uint32_t   h = 5381;

  while(klen){
      h ^= *key;
      h = ((h << 5) + h);
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

/* eof (hfunc_djbm.c) */
