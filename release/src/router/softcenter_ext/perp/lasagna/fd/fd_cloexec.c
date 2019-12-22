/* fd_cloexec.c
** utility operations on file descriptors
** wcm, 2008.01.04 - 2008.02.01
** ===
*/

#include <unistd.h>
#include <fcntl.h>

#include "fd.h"


int
fd_cloexec(int fd)
{
  return fcntl(fd, F_SETFD, FD_CLOEXEC);
}


/* EOF (fd_cloexec.c) */
