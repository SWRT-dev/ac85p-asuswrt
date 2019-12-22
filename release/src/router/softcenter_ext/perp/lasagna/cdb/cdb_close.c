/* cdb_close.c
** wcm, 2010.05.29 - 2010.10.26
** ===
*/

#include <unistd.h>
#include <errno.h>

/* libcdb: */
#include "cdb.h"


int
cdb_close(struct cdb *C)
{
  int   fd = C->fd;
  int   terrno;
  int   e = 0;

  e = cdb_clear(C);
  terrno = errno;

  close(fd);

  if(e == -1)
      errno = terrno;

  return e;
}

/* eof (cdb_close.c) */
