/* cdb_open.c
** wcm, 2010.05.29 - 2010.10.26
** ===
*/

#include <unistd.h>
#include <fcntl.h>

/* libcdb: */
#include "cdb.h"


int
cdb_open(struct cdb *C, const char *path)
{
  int  fd;

  fd = open(path, O_RDONLY | O_NONBLOCK);
  if(fd == -1)
      return -1; 

  return cdb_init(C, fd);
}


/* eof (cdb_open.c) */
