/* cdb_distance.c
** wcm, 2010.05.30 - 2010.10.26
** ===
*/

/* libc: */
#include <stdint.h>

/* libcdb: */
#include "cdb.h"

/* cdb_distance()
**   after successful cdb_find(), return "distance" from target hash slot
**   may be used as an indicator for hash efficiency
**   return:
**     0 : matching record found in target hash slot (first probe)
**     1 : matching record found 1 slot from target (second probe)
**     2 : matching record found 2 slots from target (third probe)
**     ...
*/

uint32_t
cdb_distance(struct cdb *C)
{
  if(C->sN >= C->s0)
      return C->sN - C->s0;

  /* else: */
  return (C->tslots - C->s0) + C->sN;
}


/* eof (cdb_distance.c) */
