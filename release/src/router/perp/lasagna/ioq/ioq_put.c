/* ioq_put.c
** wcm, 2004.04.21 - 2010.06.25
** ===
*/
/* stdlib: */
#include <errno.h>
/* unix: */
#include <unistd.h>

#include "buf.h"
#include "cstr.h"

#include "ioq.h"


/*
** diagram internal workings of ioq with write() operations
** 
**            0
** start:    |....................|
**            p                   n
**           buffer empty
**           p = 0
**           n = sizeof(b), ie 20
** 
**   note:
**       n is invariant!
**       unlike ioq with read(), n never changes
** 
** put 5:    |xxxxx...............|
**                 p              n
**           p = 5
**           n = 20
**           avail = n - p = 15
** 
** and so forth until put request exceeds available space in buffer
** then:
**   ioq_flush() empties buffer completely to start state
**   p reset to 0
**   put starts filling again from 0
** 
** note: conceptually, buffer is always filled from front
*/ 


/* internal declarations: */
static int write_all(int fd, void *buf, size_t to_write, ssize_t (*op)());


/*
** definitions:
*/

/* write_all()
**   perform successive write() operations on buffer as necessary
**   until all bytes written to fd or write() error
**
**   return
**      0 : success, no error
**     -1 : error write() operation, errno set
**
**   note: on error, some bytes may have been written
*/      
static
int
write_all(int fd, void *buf, size_t to_write, ssize_t (*op)())
{
  ssize_t  w = 0;

  while(to_write){
      do{
          w =  op(fd, buf, to_write);
      }while((w == -1) && (errno == EINTR));

      if(w == -1)  return -1;  /* error! */
      if(w == 0){;}            /* XXX no bytes written */

      buf += w;
      to_write -= w;
  }

  /* return:
  **    = -1, error (above)
  **    =  0, success (here)
  **
  ** note: op() should set errno!
  */

  return 0;
} 


/* ioq_flush()
**   dump internal buffer with write_all() to fd
**   return:
**     0 : success, no error
**    -1 : write() error, errno set
**
**   note: on error, some bytes may have been written
*/
int
ioq_flush(ioq_t *ioq)
{
  int to_write = ioq->p;

  if(to_write == 0) return 0;
  ioq->p = 0;

  return write_all(ioq->fd, ioq->buf, to_write, ioq->op);
}


/*
** ioq_put()
**   append data of size len to internal buffer
**   call ioq_flush() as necessary to handle all len bytes of data
**   return
**     0 : success, no error
**    -1 : write() error, errno set
*/
int
ioq_put(ioq_t *ioq, const uchar_t *data, size_t len)
{
  size_t  n = ioq->n;

  if(len > (n - ioq->p)){
      /* length of data exceeds space available in internal buffer */
      /* flush current contents of buffer: */
      if(ioq_flush(ioq) != 0){
          return -1;
      }
      /* after ioq_flush(), ioq->p == 0 */
      /* set up an efficient length for block write: */
      if(n < IOQ_BUFSIZE) n = IOQ_BUFSIZE;

      /* flush data directly until remainder will fit within internal ioq */
      while(len > ioq->n){
          /* don't write more than len! */
          if(len < n) n = len;
          if(write_all(ioq->fd, (void *)data, n, ioq->op) == -1){
              /* write() error: */
              return -1;
          }
          data += n;
          len -= n;
      }
  }

  /* now len < available space in ioq */
  /* ie, len < (ioq->n - ioq->p) */
  buf_copy(ioq->buf + ioq->p, data, len);
  ioq->p += len;

  return 0;
}



/* ioq_puts()
**   ioq_put() on nul-terminated string
**   return as ioq_put()
*/
int
ioq_puts(ioq_t *ioq, const char *s)
{
  return ioq_put(ioq, (const uchar_t *)s, cstr_len(s));
}


/* ioq_putflush()
**   flush internal buffer first, then write() data directly
**   note: internal buffer will be empty on success
*/
int
ioq_putflush(ioq_t *ioq, const uchar_t *data, size_t len)
{
  /* flush first: */
  if(ioq_flush(ioq) == -1) return -1;

  /* write() the rest */
  return write_all(ioq->fd, (void *)data, len, ioq->op);
}


/*
** ioq_putsflush()
**   ioq_putflush() on nul-terminated string
*/
int
ioq_putsflush(ioq_t *ioq, const char *s)
{
  return ioq_putflush(ioq, (const uchar_t *)s, cstr_len(s));
}


/* ioq_putfill()
**   fill internal buffer first, then flush
**   repeat as necessary
**   note: internal ioq buffer may be non-empty on return
*/
int
ioq_putfill(ioq_t *ioq, const uchar_t *data, size_t len)
{
  size_t  n;

  while(len > (n = ioq->n - ioq->p)){
      /* fill up available space in buffer: */
      buf_copy(ioq->buf + ioq->p, data, n);
      ioq->p += n;
      data += n;
      len -= n;
      /* flush: */
      if(ioq_flush(ioq) == -1) return -1;
  }
  /* now len <= available space in ioq */
  /* ie, len <= (ioq->n - ioq->p) */
  buf_copy(ioq->buf + ioq->p, data, len);
  ioq->p += len;

  return 0;
}


/* ioq_putsfill()
**   ioq_putfill() on nul-terminated string s
*/
int
ioq_putsfill(ioq_t *ioq, const char *s)
{
  return ioq_putfill(ioq, (const uchar_t *)s, cstr_len(s));
}


/* eof: ioq_put.c */
