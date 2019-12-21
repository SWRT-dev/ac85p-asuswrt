/* execvx.h
** execvx: execv*() extended
** wcm, 2009.09.09 - 2009.09.21
** ===
*/
#ifndef EXECVX_H
#define EXECVX_H

#ifndef EXECVX_PATH
#define EXECVX_PATH "/bin:/usr/bin"
#endif

/* execvx()
**   conceptually an execvpe()
**     that is, execve() with
**        * prog may be specified as a filename (rather than a pathname)
**        * argv[] list
**        * envp[] list
**     plus: optionally specify an alternative search path
**
**   exec filename prog with arglist argv[] and environment envp[]
**   if prog contains '/', considered a pathname
**   otherwise prog is considered a filename
**   if prog is a filename:
**      if optional 'search' argument is NULL:
**         it is searched for in PATH of current environment
**         if PATH undefined in current environment:
               searched in compiled-in define EXECVX_PATH
**      if 'search' is a non-NULL, PATH-like string
**         it is searched for in 'search' as if PATH of current environment
**
**   return:
**      on success, no return
**      on failure, -1 and errno set as with execve()
*/
extern
int execvx(
  const char *prog,
  char *const argv[],
  char *const envp[],
  const char *search);


#endif /* EXECVX_H */
/* eof: execvx.h */
