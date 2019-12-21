/* hdb_close.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

#include <unistd.h>
#include <errno.h>

/* libhdb: */
#include "hdb.h"


int
hdb_close(struct hdb *H)
{
  int   fd = H->fd;
  int   terrno;
  int   e = 0;

  e = hdb_clear(H);
  terrno = errno;

  close(fd);

  if(e == -1)
      errno = terrno;

  return e;
}

/* eof (hdb_close.c) */
