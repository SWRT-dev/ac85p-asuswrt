/* pidlock.h
** pidlock: runtime lockfile for single process restriction
** (implemented with posix advisory locking through padlock module)
** wcm, 2008.10.09 - 2011.02.01
** ===
*/
#ifndef PIDLOCK_H
#define PIDLOCK_H 1

#include <stddef.h>

enum pidlock_wait {
  PIDLOCK_NOW = 0,  /* "NOW", no-wait, non-blocking */
  PIDLOCK_WAIT      /* block until lock acquired */
};


/* pidlock_check()
**   check for pidlock on lockfile
**
**   return
**     > 0 : pid of process owning lock on lockfile
**     0   : pidlock not running
**    -1   : system error, errno set 
*/    
extern pid_t pidlock_check(const char *lockfile);


/* pidlock_set()
**   acquire exclusive lock with padlock on lockfile
**   if pid non-zero, write pid into lockfile after lock acquired
**   uses padlock module, exclusive lock, blocking according to lockwait
**
**   return
**     >= 0 : ok, pidlock set successfully, open fd returned
**    -1    : failure
*/
extern int pidlock_set(const char *lockfile, pid_t pid, enum pidlock_wait lockwait);


#endif /* PIDLOCK_H */
/* eof: pidlock.h */
