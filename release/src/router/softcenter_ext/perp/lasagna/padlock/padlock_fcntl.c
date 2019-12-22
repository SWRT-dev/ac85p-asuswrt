/* padlock_fcntl.c
** posix advisory locking module
** wcm, 2008.01.04 - 2008.02.11
** ===
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include "padlock.h"


int
padlock_fcntl(int fd, int cmd, struct flock *lock)
{
  int  e;

  do{
      e = fcntl(fd, cmd, lock);
  }while((e == -1) && (errno == EINTR));

  return e;
}


/* EOF (padlock_fcntl.c) */
