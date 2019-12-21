/* dynbuf.h
** dynamic (growable) buffer
** wcm, 2004.04.21 - 2012.08.04
** ===
*/
#ifndef DYNBUF_H
#define DYNBUF_H

#include <stddef.h>  /* size_t */
#include <unistd.h>  /* ssize_t */

#define DYNBUF_INITSIZE  1

struct dynbuf {
  void    *buf;
  size_t   n;   /* actual size allocated  */
  size_t   p;   /* current position (aka "length", "used") */
};
typedef struct dynbuf  dynbuf_t;

/* constructor for malloc'ed dynbuf: */
extern dynbuf_t * dynbuf_new(void); 

/* destructor for malloc'ed dynbuf: */
extern void dynbuf_free(dynbuf_t *);

/* dynbuf_INIT()
** static dynbuf object initialization
** usage:
**   dynbuf_t dyn = dynbuf_INIT();
**
** should be preferred to:
**   dynbuf_t d = {0,0,0};
*/
#define dynbuf_INIT() {NULL, 0, 0}

/* release internal buffer only: */
extern void dynbuf_freebuf(dynbuf_t *);

/* no memory release, just set d->p = 0 */
extern void dynbuf_clear(dynbuf_t *);
#define dynbuf_CLEAR(d) ((d)->p = 0)

/* accessors: */
extern void * dynbuf_buf(dynbuf_t *);
extern size_t dynbuf_len(dynbuf_t *);
/* accessor macros: */
#define dynbuf_BUF(d) ((d)->buf)
#define dynbuf_LEN(d) ((d)->p)

/* if necessary, grow to accomodate total need */
/* return -1 error, 0 no error */
extern int dynbuf_need(dynbuf_t *, size_t need);

/* if necessary, grow to accomodate additional add */
/* return -1 error, 0 no error */
extern int dynbuf_grow(dynbuf_t *, size_t add);

/* dynbuf_put*() functions:
**   "put" means _append_ to dynbuf
**
**   return -1 on error, 0 no error
*/
extern int dynbuf_put(dynbuf_t *to, const dynbuf_t *from);
extern int dynbuf_putbuf(dynbuf_t *, const void *buf, size_t len);

/* dynbuf_putb()
** append single byte to buffer
*/
extern int dynbuf_putb(dynbuf_t *, unsigned char b);

/* dynbuf_putc()
** append single char
** note:
**    requires char argument in a string, eg:
**    dynbuf_putc(d, "K")
** note:
**    **deprecated**
**    see dynbuf_putb()
*/
#define dynbuf_putc(d,c)  dynbuf_putbuf((d),&(c),1)

/* dynbuf_puts()
** str is NUL terminated,
** NUL is _not_ copied into dynbuf
*/
extern int dynbuf_puts(dynbuf_t *, const char *str);

/* append '\0' to buffer: */
extern int dynbuf_putnul(dynbuf_t *);
#define dynbuf_putNUL(d,c)  dynbuf_putb((d),'\0')

/* "copy" buf into dynbuf, overwriting previous contents: */
extern int dynbuf_copy(dynbuf_t *to, const dynbuf_t *from);
extern int dynbuf_copybuf(dynbuf_t *, const void *buf, size_t len);

/*
** str is NUL terminated,
** NUL is _not_ copied into dynbuf
*/
extern int dynbuf_copys(dynbuf_t *, const char *str);

/* dynbuf_pack()
** pack variable number of uintXX_t arguments specified in fmt into dynbuf
** fmt specification:
**   'b': 1 byte    uchar_t
**   's': 2 bytes   uint16_t
**   'd': 4 bytes   uint32_t
**   'L': 8 bytes   uint64_t
**
** return:
**   >= 0 : number of bytes packed
**     -1 : error, errno set
**
** errno values include:
**   EINVAL:  unknown specifier in fmt
**   ENOMEM:  allocation failure
**
** notes:
**   dynbuf version of upak_pack()
*/
extern ssize_t dynbuf_pack(dynbuf_t *, const char *fmt, ...);

#endif /* DYNBUF_H */
