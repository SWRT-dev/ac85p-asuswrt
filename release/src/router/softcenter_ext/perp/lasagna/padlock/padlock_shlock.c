/* padlock_shlock.c
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
padlock_shlock(int fd, enum padlock_wait wait)
{
  struct flock  lock;
  int           cmd;

  lock.l_type   = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;

  cmd = (wait == PADLOCK_WAIT) ? F_SETLKW : F_SETLK;

  return padlock_fcntl(fd, cmd, &lock);
}


/* EOF (padlock_shlock.c) */
