/* hfunc_fnva.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* fnva
**   "fowler/noll/vo" alternate hash, FNV-1a
**   params setup for 32-bit hash value: i=2166136261, m=16777619
**
**   notes:
**     FNV-1a
**     params identical to fnv1 but internal operations ordered differently
**
**     <quote>
**       Some people use FNV-1a instead of FNV-1 because they see slightly
**       better dispersion for tiny (<4 octets) chunks of memory.
**     </quote>
**
**    fnv website:
**    http://www.isthe.com/chongo/tech/comp/fnv/
*/


uint32_t
hfunc_fnva(const uchar_t *key, size_t klen)
{
  uint32_t   h = 2166136261U;

  while(klen){
      h = (h ^ *key) * 16777619U;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_fnva.c) */
