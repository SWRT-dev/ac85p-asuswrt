/* ufunc_u32add.c
** ufunc, unsigned integer utilities
** wcm, 2010.11.16 - 2010.11.16
** ===
*/
/* libc: */
#include <stdint.h>

/* unix: */
#include <errno.h>

/* lasagna: */
#include "ufunc.h"

/* ufunc_u32add()
**   update u with u += add
**
**   return:
**     0 : no error, updated value loaded into u
**    -1 : overflow, errno set ERANGE
*/
int
ufunc_u32add(uint32_t *u, uint32_t add)
{
  uint32_t  u_new = *u;

  u_new += add;
  if(u_new < add){
      errno = ERANGE;
      return -1;
  }

  *u = u_new;
  return 0;
}

/* eof: ufunc_u32add.c */
