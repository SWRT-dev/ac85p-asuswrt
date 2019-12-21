/* ufunc.h
** ufunc, unsigned integer utilities
** wcm, 2010.11.16 - 2012.07.23
** ===
*/
#ifndef UFUNC_H
#define UFUNC_H 1

#include <stdint.h>

/* ufunc_u32add()
**   update u with u += add
**
**   return:
**     0 : no error, updated value loaded into u
**    -1 : overflow, errno set ERANGE
*/
extern int ufunc_u32add(uint32_t *u, uint32_t add);


/* ufunc_u48add()
**   update u with u += add
**   contrain results to UFUNC_U48MAX (48 bits)
**
**   return:
**     0 : no error, updated value loaded into u
**    -1 : overflow, errno set ERANGE
*/
#define  UFUNC_U48MAX  (uint64_t)((1ULL<<48)-1)
extern int ufunc_u48add(uint64_t *u, uint64_t add);

#endif /* UFUNC_H */
/* eof: ufunc.h */
