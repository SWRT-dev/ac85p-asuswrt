/* cdb_hashpart.c
** incremental version of cdb_hash() function for cdbmake
** wcm, 2010.06.11 - 2010.12.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "cdb.h"

/* cdb_hashpart()
**   incremental cdb_hash() support for cdbmake
**   hash some additional part of a key
**
**   usage:
**
**       h = CDB_HASHINIT;
**       for(;;){
**           h = cdb_hashpart(h, part, partlen);
**           ...
**       }
*/


uint32_t
cdb_hashpart(uint32_t h, const uchar_t *key, size_t klen)
{
  while(klen > 3){
      /* mix operation precedes combine: */
      h += h << 5; h ^= *key++;
      h += h << 5; h ^= *key++;
      h += h << 5; h ^= *key++;
      h += h << 5; h ^= *key++;
      klen -= 4;
  }

  switch(klen){
  case 3: h += h << 5; h ^= *key++; /* fallthrough */
  case 2: h += h << 5; h ^= *key++; /* fallthrough */
  case 1: h += h << 5; h ^= *key++;
  }

  return h;
}


/* eof (cdb_hashpart.c) */
