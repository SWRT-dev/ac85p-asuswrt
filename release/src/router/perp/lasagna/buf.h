/* buf.h
** low-level buffer operations
** wcm, 2004.04.20 - 2009.07.27
** ===
*/
#ifndef BUF_H
#define BUF_H

#include <stddef.h>  /* for size_t */

/* buf_cmp()
**   compare bytes in buffers b1 and b2 upto length n
**
**   return:
**      0:  b1 matches b2
**     <0:  b1 "less than" b2
**     >0:  b1 "greater than" b2
*/
extern int buf_cmp(const void *b1, const void *b2, size_t n);

/* buf_fill()
**   initialize first n bytes of buffer b with (unsigned char)c
**   return b
*/
extern void * buf_fill(void *b, size_t n, int c);

/* convenience macros to initialize a buffer NULL: */
#define buf_zero(b,n) buf_fill((b),(n),(0))
#define buf_WIPE(b,n) buf_fill((b),(n),(0))

/* buf_copy()
**   copy count bytes in buffer "from" to buffer "to"
**   starting from beginning of buffers
**   return "to"
**   note: to and from may overlap
*/ 
extern void * buf_copy(void *to, const void *from, size_t count);

/* buf_rcopy()
**   copy count bytes in buffer "from" to buffer "to"
**   starting from count position of buffers
**   return "to"
**   note: to and from may overlap
*/ 
extern void * buf_rcopy(void *to, const void *from, size_t count);

/* buf_ndx()
**   seek position of first occurence of byte (unsigned char)c
**   within the first n bytes of buffer b
**
**   return:
**     if found:  first pos (in range 0..(n-1)), b[pos] = (unsigned char)c
**     not found: n
*/ 
extern size_t buf_ndx(const void *b, size_t n, int c);

/* buf_rndx()
**   seek position of last occurence of byte (unsigned char)c
**   within the first n bytes of buffer b
**
**   return:
**     if found:  last pos (in range 0..(n-1)), b[pos] = (unsigned char)c
**     not found: n
*/ 
extern size_t buf_rndx(const void *b, size_t n, int c);


#endif /* BUF_H */
/* eof: buf.h */
