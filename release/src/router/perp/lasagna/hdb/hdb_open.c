/* hdb_open.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

#include <unistd.h>
#include <fcntl.h>

/* libhdb: */
#include "hdb.h"


int
hdb_open(struct hdb *H, const char *path)
{
  int  fd;

  fd = open(path, O_RDONLY | O_NONBLOCK);
  if(fd == -1)
      return -1; 

  return hdb_init(H, fd);
}


/* eof (hdb_open.c) */
