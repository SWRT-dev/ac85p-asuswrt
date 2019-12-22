/* fd.h
** utility operations on file descriptors
** wcm, 2008.01.04 - 2009.09.22
** ===
*/
#ifndef FD_H
#define FD_H 1


/* fd_cloexec():
**   set FD_CLOEXEC ("close on exec") flag on fd
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int fd_cloexec(int fd);


/* fd_nonblock():
**   set O_NONBLOCK flag on fd (for non-blocking operations)
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int fd_nonblock(int fd);


/* fd_blocking():
**   unset O_NONBLOCK flag on fd (for blocking operations)
**   returns:
**     0 : success
**    -1 : failure (errno set)
*/
extern int fd_blocking(int fd);

/* fd_dupe()
**   duplicate from onto to
**   
**   return:
**      0  : success (the file descriptor to now points to from)
**     -1  : failure (errno set)
*/
extern int fd_dupe(int to, int from);

/* fd_move()
**   dupe from onto to, then close from
**   that is:
**     fd_dupe(to, from);
**     close(from);
**
**   return:
**      0  : success (the file descriptor to now points to from)
**     -1  : failure (errno set)
*/
extern int fd_move(int to, int from);


#endif /* FD_H */
/* EOF (fd.h) */
