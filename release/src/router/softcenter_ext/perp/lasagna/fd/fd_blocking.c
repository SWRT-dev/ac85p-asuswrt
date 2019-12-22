/* fd_blocking.c
** utility operations on file descriptors
** wcm, 2008.01.04 - 2008.02.01
** ===
*/

#include <unistd.h>
#include <fcntl.h>

#include "fd.h"


int
fd_blocking(int fd)
{
  return fcntl(fd, F_SETFL, ((fcntl(fd, F_GETFL, 0)) & ~O_NONBLOCK));
}


/* EOF (fd_blocking.c) */
