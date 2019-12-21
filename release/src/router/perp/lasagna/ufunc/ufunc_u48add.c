/* ufunc_u48add.c
** ufunc, unsigned integer utilities
** wcm, 2010.11.16 - 2012.07.23
** ===
*/
/* libc: */
#include <stdint.h>

/* unix: */
#include <errno.h>

/* lasagna: */
#include "ufunc.h"

/* ufunc_u48add()
**   update u with u += add
**   constrain results to 48 bits
**
**   return:
**     0 : no error, updated value loaded into u
**    -1 : overflow, errno set ERANGE
*/
int
ufunc_u48add(uint64_t *u, uint64_t add)
{
  uint64_t  u_new = *u;

  if((u_new > UFUNC_U48MAX) || (add > UFUNC_U48MAX)){
      errno = ERANGE;
      return -1;
  }

  u_new += add;
  if(u_new > UFUNC_U48MAX){
      errno = ERANGE;
      return -1;
  }

  *u = u_new;
  return 0;
}

/* eof: ufunc_u48add.c */
