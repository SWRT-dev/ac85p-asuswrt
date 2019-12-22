/* padlock_extest.c
** posix advisory locking module
** wcm, 2008.01.04 - 2008.02.11
** ===
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include "padlock.h"


pid_t
padlock_extest(int fd)
{
  struct flock  lock;
  int           e;

  lock.l_type   = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;

  e = padlock_fcntl(fd, F_GETLK, &lock);
  /* error: */
  if(e < 0)
      return (pid_t)e;

  return ((lock.l_type == F_UNLCK) ? 0 : lock.l_pid);
}


/* EOF (padlock_extest.c) */
