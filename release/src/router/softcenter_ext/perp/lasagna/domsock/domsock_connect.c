/* domsock_connect.c
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
domsock_connect(const char *path)
{
  struct sockaddr_un  sockaddr;
  int                 s;
  int                 terrno;

  if(sizeof(sockaddr.sun_path) < (cstr_len(path) + 1)){
      errno = ENAMETOOLONG;
      return -1;
  }

  if((s = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){
      return -1;
  }

  buf_zero(&sockaddr, sizeof (sockaddr));
  sockaddr.sun_family = AF_LOCAL;
  cstr_copy(sockaddr.sun_path, path);

  if(connect(s, (const struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1){
      terrno = errno;
      close(s);
      errno = terrno;
      return -1;
  }

  /* success: */
  return s;
}

/* eof: domsock_connect.c */
