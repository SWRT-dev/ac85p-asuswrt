/* devout.h
** devout: simple unbuffered variable argument output
** intended for sparing use, such as for minimalist stderr reporting
** warning: as many write() syscalls as arguments!
** wcm, 2009.11.25 - 2009.11.30
** ===
*/
#ifndef DEVOUT_H
#define DEVOUT_H

#include <stddef.h>  /* for size_t */
#include <unistd.h>

/* devout_()
** write one or more non-null argument strings to file descriptor
** return:
**   >0 : total number of bytes written
**   -1 : error during write(), errno set
**
** note: caller must terminate variable argument list with NULL!
**
** >> don't use this function!
** >> use the devout() macro instead
*/
extern ssize_t devout_(int fd, const char *s0, ...);

/* devout()
** write one or more non-null argument strings to file descriptor
*/
#define devout(fd, ...) \
  devout_((fd), __VA_ARGS__, NULL)


#endif /* DEVOUT_H */
/* eof: devout.h */
