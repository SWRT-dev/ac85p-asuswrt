/* hfunc_ghfm.c
** wcm, 2010.06.11 - 2010.06.11
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* ghfm()
**   generalized hash function:
**
**   combine preceeds mix:
**     combine operation is xor
**     mix operation is mult
**
**   post processing:
**     sequence of adds/shifts/xors
**
**   note:
**     this is a generalized version of FNV-1a,
**     with Bret Mulvey's recommended post-processing sequence as
**     used in hfunc_fnvm()
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
**   see: http://bretm.home.comcast.net/~bretm/hash/6.html
*/


uint32_t
hfunc_ghfm(const uchar_t *key, size_t klen, uint32_t init, uint32_t mult)
{
  uint32_t   h = init;

  while(klen){
      /* combine: */
      h ^= *key;
      /* mix: */
      h *= mult;
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

/* eof (hfunc_ghfm.c) */
