/* hfunc_postmix32.c
** wcm, 2010.06.11 - 2010.06.11
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "hfunc.h"

/* postmix32()
**   apply post-process mixing to hashed value h
**
**   notes:
**     * not a hash function in itself
**
**     * used for post-process mixing of a hash result that
**       doesn't otherwise provide it's own post-processing
**
**     * based on mods to fnv-1a suggested by bret mulvey[*8],
**       and as implemented in fnvm(), ghfm()
*
**   see: http://bretm.home.comcast.net/~bretm/hash/6.html
*/


uint32_t
hfunc_postmix32(uint32_t h)
{
  /* post-processing: */
  h += h << 13;
  h ^= h >> 7;
  h += h << 3;
  h ^= h >> 17;
  h += h << 5;

  return h;
}

/* eof (hfunc_postmix32.c) */
