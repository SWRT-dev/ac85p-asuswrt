/* hfunc_fnvm.c
** wcm, 2010.05.27 - 2010.06.11
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* fnvm()
**   modified FNV-1a
**   developed by Bret Mulvey:
**   http://bretm.home.comcast.net/~bretm/hash/6.html
**
**   <quote>
**     If you want to use an FNV-style hash function, I recommend using
**     the modified version listed in Figure 4. This version passes
**     all the uniform distribution tests above and it achieves avalanche
**     for every tested combination of input and output bits (green
**     squares everywhere). No xor-folding step is required.
**     
**     The only difference between this version and the original version
**     is that the mixing steps occurs after the combining step in each
**     round, and it adds a post-processing step to mix the bits even
**     further. These two changes completely correct the avalanche
**     behavior of the function. As a result, this version of FNV passes
**     all of the #2 tests above, all the way up to 216 buckets. I
**     haven't tested larger sizes but I suspect it would be OK there
**     as well.
**   </quote>     
**
** FNV-1a notes:
**     params identical to FNV-1, but internal operations ordered differently
**
**     <quote>
**       Some people use FNV-1a instead of FNV-1 because they see slightly
**       better dispersion for tiny (<4 octets) chunks of memory.
**     </quote>
**
**    fnv website:
**    http://www.isthe.com/chongo/tech/comp/fnv/
*/


/* 2166136261: */
#define FNV_32_BASIS ((uint32_t)0x0811c9dc5)
/* 16777619: */
#define FNV_32_PRIME ((uint32_t)0x01000193)


uint32_t
hfunc_fnvm(const uchar_t *key, size_t klen)
{
  uint32_t   h = FNV_32_BASIS;

  while(klen){
      /* combine: */
      h ^= *key;
      /* mix: */
      h *= FNV_32_PRIME;
#if 0
      /* mix factor above in shift expressions: */
      h += (h<<1) + (h<<4) + (h<<7) + (h<<8) + (h<<24);
#endif
      ++key;
      --klen;
  }

  /* post-processing: */
  h += h << 13;
  h ^= h >> 7;
  h += h << 3;
  h ^= h >> 17;
  h += h << 5;

  return h;
}

/* eof (hfunc_fnvm.c) */
