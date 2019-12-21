/* execvx.c
** wcm, 2009.09.09 - 2009.09.13
** ===
*/
/* stdlib: */
#include <errno.h>
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <limits.h>

#include "cstr.h"
#include "dynstr.h"

#include "execvx.h"

/* constrain path size (imperfect): */
#ifndef PATH_MAX
#define PATH_MAX  1024
#endif


int
execvx
(
  const char   *prog,
  char *const   argv[],
  char *const   envp[],
  const char   *search )
{
  const char  *path = NULL;
  dynstr_t     tpath = dynstr_INIT();
  size_t       tok;
  int          last_errno = 0;

  /* short-circuit on prog = "" */
  if(prog[0] == '\0'){
      errno = ENOENT;
      return -1;
  }

  /* prog a pathname? */
  tok = cstr_pos(prog, '/');
  if(prog[tok] != '\0'){
      return execve(prog, argv, envp);
  }

  /* setup search path for prog: */
  if(search != NULL){
      path = search;
  } else if((path = getenv("PATH")) == NULL){
      path = EXECVX_PATH;
  }

  while(path[0] != '\0'){

      /* recycle any dynstr already allocated: */
      dynstr_CLEAR(&tpath);

      /* seek end of this path element: */
      tok = cstr_pos(path, ':');

      /* note: dynstr* functions set ENOMEM on failure: */
      if(tok > 0){
          /* path element defined: */
          if(dynstr_putn(&tpath, path, tok) == -1)
              break;
      } else {
          /* tok = 0 iff path[0] = ':'
          ** (path[0] = '\0' trapped at top of loop)
          ** search current directory:
          */
          if(dynstr_puts(&tpath, ".") == -1)
              break;
      }

      if(dynstr_vputs(&tpath, "/", prog) == -1)
          break;

      /* inhibit excessive pathnames as potential security issue: */
      if(dynstr_LEN(&tpath) > PATH_MAX){
          errno = ENAMETOOLONG;
          /* XXX, continue or fail now ??? */
          /* skip this exec(), try next path element: */
          goto nextpath;
      }

      /* let's try it: */
      execve(dynstr_STR(&tpath), argv, envp);

      /* nope, execve() failure: */
      if(errno != ENOENT){
          /* save non-ENOENT errno for reporting on exit: */
          last_errno = errno;
          if(! ((errno == EACCES) || (errno == EPERM) || (errno == EISDIR))){
              /* fail now (eg, ENOTDIR, ELOOP, ENOEXEC, etc): */
              break;
          }
      }

nextpath:
      /* note: see comments at end of this file */
      if(tok == 0){
          path += 1;
      } else {
      /* tok > 0 : */
          path += tok;
          if((path[0] == ':') && (path[1] != '\0')){
              path += 1;
          }
      }

  }/*end while()*/

  /* here on fail: */
  if(last_errno != 0)
      errno = last_errno;

  /* release any allocated dynstr: */
  dynstr_freestr(&tpath);

  return -1;
} 


#if 0
note: interpreting current directory in PATH:

   ":/bin:/usr/bin"  (colon at beginning of PATH)
   "/bin::/usr/bin"  (sequence of two colons)
   "/bin:/usr/bin:"  (colon at end of PATH)

(reference: APUE(2e), page 232)

note: comments on PATH processing algorithm

      ...
      /* terminal condition in loop: end of path */
      while(path[0] != '\0'){
          ...

          /* seek end of current path element: */
          tok = cstr_pos(path, ':');

          /* 2 cases: tok = 0 or tok > 0
          ** tok == 0 iff path[0] == ':'
          **    ie consider current directory
          ** for tok > 0, 2 cases:
          **     path[tok] = '\0', this is the last path element
          **     or
          **     path[tok] = ':', another path element follows
          */

          ...
          execve(...);
          ...

       nextpath:
          if(tok == 0){
              /* tok == 0 iff path[0] == ':'
              ** (ie, we have just considered current directory)
              ** increment path to next potential path element:
              */
              path += 1;
          } else {
              /* tok > 0 */
              /* first: advance path to end of current path element: */
              path += tok;
              /* now 3 cases here:
              **   path[0] == '\0'
              **       do nothing
              **       (terminal condition)
              **   path[0] == ':' && path[1] == '\0'
              **       do nothing
              **       (a final ':' to be considered as current directory)
              **   path[0] == ':' && path[1] != '\0'
              **       another path element follows
              **       increment path to next path element
              */
              if((path[0] == ':') && (path[1] != '\0')){
                  path += 1;
              }
          }/*end while()*/
#endif


/* eof: execvx.c */
