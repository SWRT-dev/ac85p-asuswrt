/* domsock_create.c
** domsock: unix/local domain sockets
** wcm, 2010.12.30 - 2010.12.30
** ===
*/

/* libc: */

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

/* lasagna: */
#include "buf.h"
#include "cstr.h"

/* domsock: */
#include "domsock.h"

int
domsock_create(const char *path, mode_t mode)
{
  struct sockaddr_un  sockaddr;
  mode_t              umask_orig;
  int                 s;
  int                 e, terrno;

  if(sizeof(sockaddr.sun_path) < (cstr_len(path) + 1)){
      errno = ENAMETOOLONG;
      return -1;
  }

  if((s = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){
      return -1;
  }

  unlink(path);
  buf_zero(&sockaddr, sizeof (sockaddr));
  sockaddr.sun_family = AF_LOCAL;
  cstr_copy(sockaddr.sun_path, path);
  if(bind(s, (const struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1){
      goto FAIL;
  } 

  if(mode != 0){
      umask_orig = umask(0);
      e = chmod(path, mode);
      umask(umask_orig);
      if(e == -1){
          goto FAIL;
      }
  }

  /* success: */
  return s;

FAIL:
  terrno = errno;
  unlink(path);
  close(s);
  errno = terrno;
  return -1;
}


/* eof: domsock_create.c */
