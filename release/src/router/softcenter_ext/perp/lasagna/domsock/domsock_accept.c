/* domsock_accept.c
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
domsock_accept(int s)
{
  struct sockaddr_un  connaddr;
  socklen_t           connlen;

  connlen = sizeof(connaddr); 
  return accept(s, (struct sockaddr *)&connaddr, &connlen);
}

/* eof: domsock_accept.c */
