/* domsock.h
** domsock: interface to unix/local domain sockets
** wcm, 2008.01.04 - 2010.12.30
** ===
*/
#ifndef DOMSOCK_H
#define DOMSOCK_H

#include <sys/types.h> /* mode_t */
#include <sys/socket.h>

/*
** domsock: unix/local domain socket interface module
** (supporting SOCK_STREAM (TCP) byte stream connections only)
*/

/* domsock_create()
**   create and bind a unix/local domain socket at path
**   if (mode != 0), set chmod(path, mode)
**
**   return:
**     >=0 : success, file descriptor of socket
**      -1 : error, errno set
**
**   notes:
**     intended for "server"
**     path should be absolute
**       --this function does not constrain, but should?
*/
extern
int domsock_create(const char *path, mode_t mode);


/* domsock_listen()
**   convenience macro/alias for listen()
**
**   notes:
**     intended for "server"
**     called after domsock_create()
*/
#define domsock_listen(s,q)  listen((s),(q))


/* domsock_accept()
**   accept incoming connection on domsock socket s
**
**   return:
**     >=0 : success, file descriptor for connected socket
**      -1 : error, errno set
**
**   notes:
**     intended for "server"
**     called after domsock_create(), domsock_listen()
**     "hides" client sockaddr/socklen parameters returned by accept()
**       --interface simplification
**       --normally not used (??)
*/
extern
int domsock_accept(int s);


/* domsock_close()
**   close() domsock socket s and unlink() its path address
**   
**   return:
**     0: success
**    -1: error, errno set
**
**   notes:
**     intended for "server", the process using domsock_create()
*/
extern
int domsock_close(int s);


/* domsock_connect()
**   establishes socket connected to the listening domsock at path
**
**   return:
**     >=0 : success, file descriptor of connected socket
**      -1 : error, errno set
**
**   notes:
**     intended for "client"
*/
extern
int domsock_connect(const char *path);


#endif /* DOMSOCK_H */
/* eof: domsock.h */
