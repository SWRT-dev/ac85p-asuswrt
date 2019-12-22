/* hdb_distance.c
** wcm, 2010.05.30 - 2010.12.14
** ===
*/

/* libc: */
#include <stdint.h>

/* libhdb: */
#include "hdb.h"

/* hdb_distance()
**   after successful hdb_find(), return "distance" from target hash slot
**   may be used as an indicator for hash efficiency
**   return:
**     0 : matching record found in target hash slot (first probe)
**     1 : matching record found 1 slot from target (second probe)
**     2 : matching record found 2 slots from target (third probe)
**     ...
*/

uint32_t
hdb_distance(struct hdb *H)
{
  if(H->sN >= H->s0)
      return H->sN - H->s0;

  /* else: */
  return (H->tslots - H->s0) + H->sN;
}


/* eof (hdb_distance.c) */
