/* hfunc_sdbm.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* sdbm
** sdbm: ghfa with i=0, m=65599
** original attribution?
**   sdbm hash function, ozan yigit
**   also in gawk
**   also "red dragon book"
**
** note:
** this version uses bit shift/add operation for multiplier
**     (h * 65599) == ((h<<6) + (h<<16) - h)
**
** ie:  2^6 + 2^65536 - 1 = 65599
**
*/

uint32_t
hfunc_sdbm(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 6) + (h << 16) - h) + *key;
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_sdbm.c) */
