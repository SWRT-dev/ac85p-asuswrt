/* hdb_clear.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

/* lasagna: */
#include "buf.h"

/* libhdb: */
#include "hdb.h"


int
hdb_clear(struct hdb *H)
{
  int   e = 0;

  if(H->map != 0){
      e = munmap(H->map, H->map_size);
  }

  buf_zero(H, sizeof (struct hdb));

  return e;
}

/* eof (hdb_clear.c) */
