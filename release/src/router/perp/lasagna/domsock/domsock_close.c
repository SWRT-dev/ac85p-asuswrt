/* domsock_close.c
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

/* domsock: */
#include "domsock.h"

int
domsock_close(int s)
{
  struct sockaddr_un  sockaddr;
  socklen_t           len;

  if(getsockname(s, (struct sockaddr *)&sockaddr, &len) == -1){
      return -1;
  }

  close(s);

  return unlink(sockaddr.sun_path);
}

/* eof: domsock_close.c */
