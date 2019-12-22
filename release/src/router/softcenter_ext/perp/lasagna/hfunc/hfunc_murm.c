/* hfunc_murm.c
** "MurmurHash2"
** hash algorithm by austin appleby
** lasagna implementation
** wcm, 2010.06.06 - 2010.06.07
** ===
*/

#include <stddef.h>
#include <stdint.h>

/* lasagna: */
#include "uchar.h"
#include "upak.h"

#include "hfunc.h"

/* hfunc_murm()
** "MurmurHash2"
** algorigthm by Austin Appleby
** http://sites.google.com/site/murmurhash/
**
** per website, 2010.06.06:
** <quote>
**   All code is released to the public domain. For business purposes,
**   Murmurhash is under the MIT license.
** </quote>
*/

/* lasagna implementation variances:
**   * this version should produce consistent results across platforms
**     (the original won't)
**
**   * this version does *not* implement a random seed argument for
**     hash initialization
**     (uses prime constant picked out of my a**)
*/

/* hash function: */
uint32_t
hfunc_murm(const uchar_t *key, size_t klen)
{
  /* hash initialization (substituting klen for seed argument): */
  uint32_t   h = 0xdeafb0faU;
  /* "mixing constants": */
  const uint32_t  m = 0x5bd1e995U;
  const int       r = 24;
  /* 4-byte key workchunk: */
  uint32_t   k;

  /* main loop processes 4-byte chunks of key at a time
  **
  ** notes:
  **   loop sentinal ensures at least 4 bytes remain in key
  **   upak32_UNPACK() on 4-byte chunks of key is consistent across
  **   architectures 
  */
  while(klen >= 4){
      k = upak32_UNPACK(key);
      k *= m; 
      k ^= k >> r; 
      k *= m; 
        
      h *= m; 
      h ^= k;

      key += 4;
      klen -= 4;
  }

  /* process any klen remaining bytes (note fallthrough): */
  switch(klen){
  case 3:
      h ^= key[2] << 16;
  case 2:
      h ^= key[1] << 8;
  case 1:
      h ^= key[0];
      h *= m;
  };

  /* final mixing for last few bytes: */
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 


/* eof (hfunc_murm.c) */
