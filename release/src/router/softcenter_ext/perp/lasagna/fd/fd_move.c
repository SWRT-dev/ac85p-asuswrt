/* fd_move.c
** utility operations on file descriptors
** wcm, 2008.01.04 - 2009.09.22
** ===
*/

#include <unistd.h>
#include <fcntl.h>

#include "fd.h"


int
fd_move(int to, int from)
{
  if(to == from)
      return 0;

  /* check from before closing to: */
  if(fcntl(from, F_GETFL, 0) == -1)
      return -1;

  close(to);
  if(fcntl(from, F_DUPFD, to) == -1)
      return -1;

  close(from);
  return 0;
}


/* EOF (fd_move.c) */
