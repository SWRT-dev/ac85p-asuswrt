/* ioq.h
** ioq: buffered ("queued") io operations
** wcm, 2004.04.21 - 2012.07.24
** ===
*/
#ifndef IOQ_H
#define IOQ_H

#include <stddef.h>  /* for size_t */
#include <unistd.h>

#include "uchar.h"

/* ioq_getln() into dynstr: */
#include "dynstr.h"


struct ioq {
  int        fd;     /* file descriptor */
  uchar_t   *buf;    /* internal buffer */
  size_t     n;      /* buffer length   */
  size_t     p;      /* position */
  ssize_t  (*op)();  /* io operation */
};
typedef struct ioq  ioq_t;

/* ioq_init()
**   initialize an ioq_t object
*/
extern void ioq_init(ioq_t *ioq, int fd, uchar_t *buf, size_t len, ssize_t (*op)());

/* ioq_INIT()
**   djb idiom for macro initialization of static ioq_t object, like so:
**
**     ioq_t  ioq = ioq_INIT(fd, buf, sizeof buf, &op);
*/
#define ioq_INIT(fd,buf,len,op) {(fd), (buf), (len), 0, (op) }


/* note:
**
** an ioq_t object can be used for either an input or output buffer,
** depending on the op() pointer it is setup with:
**
**   ioq_t object for input:  setup with read()-type function pointer
**   ioq_t object for output: setup with write()-type function pointer
**
** for an input ioq, use "get" functions
** for an output ioq, use "put" functions
**
** although input/output ioqs share the same data structure,
** the internal semantics are not identical
** and the functions applied to them are not symmetrical
*/

/*
**  read()-type operations
*/

/* ioq_get()
**   copy upto len bytes from ioq into data
**   return
**     > 0: success, number of bytes copied into data
**     = 0: eof from read() operation
**     < 0: error from read() operation, errno set
**
**   note:
**      call may satisfy less than the len bytes requested
**      this does not mean a "short" read()
**      it only indicates what the internal state of ioq could supply
*/
extern ssize_t ioq_get(ioq_t *ioq, uchar_t *data, size_t len);

/* ioq_getc()
**   copy one byte from ioq into c
**   return as ioq_get()
**
**   usage note: c argument is an address of a char:
**
**     e = ioq_getc(ioq, &c);
**
**   think prototype:
**
**     int ioq_getc(ioq_t q, uchar_t *c);
**
*/
#define ioq_getc(ioq,c) \
    ioq_get((ioq),(uchar_t *)(c),1)

/* ioq_GETC()
**   macro version of ioq_getc()
*/
#define ioq_GETC(ioq,c) \
  ( ((ioq)->p > 0) \
    ? *((uchar_t *)(c)) = (ioq)->buf[(ioq)->n], (ioq)->p -= 1, (ioq)->n += 1, 1 \
    : ioq_get((ioq),(uchar_t *)(c),1) \
  )


/* ioq_getln()
**   read from ioq into dynstr S upto and including '\n'
**
**   return
**     -1 : error (io or malloc), errno set
**      0 : eof (S may be of non-zero length, will not incl sep)
**      1 : ok  (S will include sep, get length out of S)
*/
extern int ioq_getln(ioq_t *ioq, dynstr_t *S);


/*
** low-level "get" operations:
**   see remarks following declarations for suggested idiom
*/

/* ioq_feed()
**   if internal buffer is empty, try a read() operation to fill it
**   return
**     > 0 : bytes available in internal buffer
**     = 0 : eof from read()
**     < 0 : error from read(), errno set
*/
extern ssize_t ioq_feed(ioq_t *ioq);

/* ioq_peek()
**   return pointer to first byte of data within internal buffer
*/
extern void * ioq_peek(ioq_t *ioq);

/* ioq_PEEK()
**   macro version of ioq_peek()
*/
#define ioq_PEEK(ioq) \
  ((ioq)->buf + (ioq)->n)


/* ioq_seek()
**   advance pos bytes within internal data buffer
**
**   important: caller must constrain value of pos:
**       ((pos >= 0) && (pos < ioq_feed(ioq)))
**     
*/   
extern void ioq_seek(ioq_t *ioq, size_t pos);

/* ioq_SEEK()
**   macro version of ioq_seek()
*/
#define ioq_SEEK(ioq,nn) \
  { (ioq)->p -= (nn); (ioq)->n += (nn); }


/*
** suggested idiom for low-level "get" operations:
** 
**     ioq_t   in;
**     char   *b;
**     int     r;
**     ...
**     for(;;){
**         r = ioq_feed(&in);
**         if(r <= 0){ // eof or error }
**         b = ioq_peek(&in);
**         // operate directly on internal buffer of ioq:
**         doit(b, r);
**         // advance internal position within buffer of ioq:
**         ioq_seek(&in, r);
**     }
**     
*/


/*
** write()-type operations
*/

/* ioq_flush()
**   apply write() on internal buffer to fd until empty
**   return
**     0 : success, no error
**    -1 : write() error, errno set
*/
extern int ioq_flush(ioq_t *ioq);

/* ioq_put()
**   append data of size len to internal buffer
**   calls ioq_flush() as needed to handle all len bytes of data
**   return
**     0 : success, no error
**    -1 : write() error, errno set
*/
extern int ioq_put(ioq_t *ioq, const uchar_t *data, size_t len);

/* ioq_putfd()
**   append contents of open file descriptor fd to internal buffer
**   calls ioq_flush() as needed to handle all bytes of file
**   return
**     0 : success, no error
**    -1 : mmap(), or write() error, errno set
**
**   notes:
**     caller supplies size of open/readable fd from stat() call
**     uses mmap()
**     does not perform final ioq_flush()
**     does not close() fd
*/
extern int ioq_putfd(ioq_t *ioq, int fd, size_t len);

/* ioq_putfile()
**   append contents of filename to internal buffer
**   calls ioq_flush() as needed to handle all bytes of file
**   return
**     0 : success, no error
**    -1 : open(), stat(), mmap(), or write() error, errno set
**
**   notes:
**     calls open(), stat(), close(), etc.
**     calls ioq_putfd()
**     uses mmap()
**     does not perform final ioq_flush()
*/
extern int ioq_putfile(ioq_t *ioq, const char *filename);

/* ioq_puts()
**   ioq_put() on nul-terminated string s
**   return as ioq_put()
*/
extern int ioq_puts(ioq_t *ioq, const char *s);

/* ioq_putc()
**   ioq_put() single byte c
**
**   usage as: e = ioq_putc(ioq_t ioq, uchar_t c)
*/
#define ioq_putc(ioq,c) ioq_put((ioq),&(c),1)

/* ioq_PUTC()
**   macro verion of ioq_putc()
*/
#define ioq_PUTC(ioq,c) \
  ( ((ioq)->p < (ioq)->n) \
    ? ( (ioq)->buf[(ioq)->p++] = (c), 0 ) \
    : ioq_put((ioq),&(c),1) \
  )

/* ioq_vputs_()
**   ioq_puts() one or more argument strings into ioq
**   return: as ioq_put()
**   
**   notes:
**     * caller _must_ supply NULL as final argument
**     * use macro verion instead
*/
extern int ioq_vputs_(ioq_t *ioq, const char *s, ...);

/* ioq_vputs()
**   convenience macro for ioq_vputs_()
*/
#define ioq_vputs(ioq, ...)  ioq_vputs_((ioq), __VA_ARGS__, NULL)


/* "flush first" operations: */

/* ioq_putflush()
**   flush internal buffer, then write data directly
*/
extern int ioq_putflush(ioq_t *ioq, const uchar_t *data, size_t len);

/* ioq_putsflush()
**   ioq_putflush() on nul-terminated string s
*/
extern int ioq_putsflush(ioq_t *buf, const char *s);

/* "fill first" operations:
** append data to internal buffer until full
** only ioq_flush() when buffer is full
** internal buffer may be non-empty on return
**
** for compatibility with djb buffer_putalign()
*/
extern int ioq_putfill(ioq_t *ioq, const uchar_t *data, size_t len);
extern int ioq_putsfill(ioq_t *ioq, const char *s);


/* not implemented in this release: */
#if 0
/*
** "getnets": get netstring (using dynbuf):
*/
extern int ioq_getnets(ioq_t *ibuf, dynbuf *dyn, size_t max);
#endif


/* internal defines: */
#define IOQ_BUFSIZE  8192


#endif /* IOQ_H */
/* eof: ioq.h */
