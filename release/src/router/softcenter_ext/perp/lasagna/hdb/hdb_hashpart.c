/* hdb_hashpart.c
** wcm, 2010.06.11 - 2010.12.14
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hdb.h"


/* hdb_hashpart()
**   incremental hdb_hash() support for hdbmk
**   hash some additional part of a key
**
**   usage:
**
**       h = HDB_HASHINIT;
**       for(;;){
**           h = hdb_hashpart(h, part, partlen);
**           ...
**       }
*/

uint32_t
hdb_hashpart(uint32_t h, const uchar_t *key, size_t klen)
{
  while(klen > 3){
      /* combine precedes mix; mix multiplier 37: */
      h ^= *key++; h += (h << 2) + (h << 5);
      h ^= *key++; h += (h << 2) + (h << 5);
      h ^= *key++; h += (h << 2) + (h << 5);
      h ^= *key++; h += (h << 2) + (h << 5);
      klen -= 4;
  }

  switch(klen){
  case 3: h ^= *key++; h += (h << 2) + (h << 5); /* fallthrough */
  case 2: h ^= *key++; h += (h << 2) + (h << 5); /* fallthrough */
  case 1: h ^= *key++; h += (h << 2) + (h << 5);
  }

  return h;
}


/* eof (hdb_hashpart.c) */
