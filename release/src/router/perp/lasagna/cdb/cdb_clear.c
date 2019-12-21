/* cdb_clear.c
** wcm, 2010.05.29 - 2010.10.26
** ===
*/

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

/* lasagna: */
#include "buf.h"

/* libcdb: */
#include "cdb.h"


int
cdb_clear(struct cdb *C)
{
  int   e = 0;

  if(C->map != 0){
      e = munmap(C->map, C->map_size);
  }

  buf_zero(C, sizeof (struct cdb));

  return e;
}

/* eof (cdb_clear.c) */
