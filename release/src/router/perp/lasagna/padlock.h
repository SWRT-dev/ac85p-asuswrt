/* padlock.h
** posix advisory locking module
** wcm, 2008.01.04 - 2008.10.23
** ===
*/
#ifndef PADLOCK_H
#define PADLOCK_H 1

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>


/*
** posix advisory lock interface
** (using fcntl() internals)
*/

enum padlock_wait {
  PADLOCK_NOW = 0, /* "NOW", "no-wait", non-blocking  */
  PADLOCK_WAIT     /* block until lock acquired */
};

/* frontend for fcntl: */
extern int padlock_fcntl(int fd, int cmd, struct flock *lock);

/* padlock_exlock():
**   acquire exclusive advisory lock on fd, wait as specified
**   note: process must have fd open() for writing
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_exlock(int fd, enum padlock_wait wait);

/* padlock_shlock():
**   acquire shared advisory lock on fd, wait as specified
**   note: process must have fd open() for reading
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_shlock(int fd, enum padlock_wait wait);

/* padlock_unlock():
**   release advisory lock held on fd, wait as specified
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_unlock(int fd, enum padlock_wait wait);


/* padlock_extest():
**   test for presence of exclusive advisory lock on fd
**   note: process _should_ have fd open() for writing
**   (but it doesn't seem to matter)
**   returns:
**     0 : fd is not locked
**    >0 : pid of process holding lock on fd
**    -1 : failure (errno set)
*/
extern pid_t padlock_extest(int fd);

/* padlock_shtest():
**   test for presence of shared advisory lock on fd
**   note: process _should_ have fd open() for reading
**   (but it doesn't seem to matter)
**   returns:
**     0 : fd is not locked
**    >0 : pid of process holding lock on fd
**    -1 : failure (errno set)
*/
extern pid_t padlock_shtest(int fd);


/* padlock_*byte() interface:
**
** acquire/release locks on single bytes within a file
** (use single file to manage multiple cooperative locks)
*/

/* padlock_exbyte():
**   aquire exclusive advisory lock on the single byte at offset n in fd
**   wait as specified
**   note: process must have fd open() for writing
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_exbyte(int fd, off_t n, enum padlock_wait wait);

/* padlock_shbyte():
**   aquire shared advisory lock on the single byte at offset n in fd
**   wait as specified
**   note: process must have fd open() for reading
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_shbyte(int fd, off_t n, enum padlock_wait wait);

/* padlock_unbyte():
**   release advisory lock held on the single byte at offset n in fd
**   wait as specified
**     0 : success
**    -1 : failure (errno set)
*/
extern int padlock_unbyte(int fd, off_t n, enum padlock_wait wait);


#endif /* PADLOCK_H */
/* EOF (padlock.h) */
