/* ioq_get.c
** ioq read operations
** wcm, 2004.04.21 - 2009.10.05
** ===
*/
/* stdlib: */
#include <errno.h>
/* unix: */
#include <unistd.h>

/* lasagna: */
#include "buf.h"
#include "cstr.h"

#include "ioq.h"

/*
** diagram internal workings of ioq with read() operations
** 
**            0
** start:    |....................|
**            p                   n
**           buffer empty
**           p = 0
**           n = sizeof(b), ie 20
** 
**            0
** feed:     |xxxxxxxxxxxxxxxxxxxx|
**            n                   p
**           call read() op to fill buffer
**           r = 20 bytes
**           p = 20, ie &b+20
**           n -= r, ie 0
** 
**            0
** get 5:    |.....xxxxxxxxxxxxxxx|
**                 n         p
**           r = 5 bytes copied out
**           p -= 5, ie 15 (and 15 bytes remaining)
**           n += 5, ie 5
** 
**            0
** get 12:   |.................xxx|
**               p             n  
**           r = 12 bytes copied out
**           p -= 12, ie 3 (and 3 bytes remaining)
**           n += 12, ie 17
** 
**            0
** get 5:    |....................|
**            p                   n 
**           r = 3 bytes copied out (of 5 requested)
**           p -= 3, ie 0 (and 0 bytes remaining)
**           n += 3, ie 20
** 
** internal buffer is now empty
** the next get operation will call feed() to refill it
**
** note: conceptually, internal buffer is always filled from end
**   that is, with a short read() during an ioq_feed(), the data
**   read will be repositioned so that it ends at the end of the
**   buffer
*/           

/* internal declarations: */
static ssize_t read_once(int fd, void *buf, size_t to_read, ssize_t (*op)());
static size_t ioq_extract(ioq_t *ioq, void *data, size_t len);


/*
** definitions:
*/

/* read_once()
** try single read() operation of at most want bytes from fd into buf
** return
**   result of read() operation
*/
static
ssize_t
read_once(int fd, void *buf, size_t want, ssize_t (*op)())
{
  ssize_t r;

  do{
      r = op(fd, buf, want);
  }while((r == -1) && (errno == EINTR));

  /* return:
  **   = -1, error
  **   >= 0, bytes read
  */

  return r;
}

/* ioq_extract()
** extract upto len bytes from ioq buffer into data
** return
**   the number of bytes extracted
*/
static
size_t
ioq_extract(ioq_t *ioq, void *data, size_t len)
{
  if(len > ioq->p){
      /* request is more than available, adjust it: */
      len = ioq->p;
  }

  buf_copy(data, (ioq->buf + ioq->n), len);
  ioq->p -= len;
  ioq->n += len;

  return len;
}


/* ioq_get()
**   copy upto len bytes from ioq buffer into data
**   return
**     >0 : number of bytes copied into data
**      0 : eof from fd
**     <0 : error from read() op
**
**   note:
**   call may satisfy less than len bytes requested
**   this does not indicate a "short" read()
**   only what current internal state of ioq could supply
*/ 
ssize_t
ioq_get(ioq_t *ioq, uchar_t *data, size_t len)
{
  ssize_t r;

  if(ioq->p > 0){
      /* internal buffer not empty: 
      **   extract at least what is available
      */
      return ioq_extract(ioq, data, len);
  }

  /* internal buffer empty: */
  if(ioq->n <= len){
      /* caller wants more than internal buffer size:
      **   satisfy directly with a read() operation
      */
      return read_once(ioq->fd, data, len, ioq->op);
  }

  /* caller wants less than internal buffer size:
  **   feed ioq to fill internal buffer
  **   extract requested 
  */
  r = ioq_feed(ioq);
  if(r <= 0){
      /* eof or error from read(): */
      return r;
  }

  return (ssize_t)ioq_extract(ioq, data, len);
}


/* ioq_feed()
**   if internal buffer is empty, try to fill it with read() operation
**   return
**     >0 : internal buffer not empty: ioq->p
**     >0 : internal buffer was empty: results of read() operation
**      0 : internal buffer was empty: eof from read() operation
**     <0 : internal buffer was empty: error from read() operation
*/
ssize_t
ioq_feed(ioq_t *ioq)
{
  ssize_t r;

  /* internal buffer not empty: */
  if(ioq->p){
      return ioq->p;
  }

  /* buffer empty, do a read(): */
  r = read_once(ioq->fd, ioq->buf, ioq->n, ioq->op);
  if(r <= 0){
      /* eof or error from read(): */
      return r;
  }

  /* update positions: */
  ioq->p = r;
  ioq->n -= r;

  if(ioq->n > 0){
      /* got a short read(),
      **   slide data read to the end of internal buffer:
      */
      buf_rcopy((ioq->buf + ioq->n), ioq->buf, (size_t)r);
  }

  return r;
}


/* ioq_peek()
**   return pointer to current first byte of internal buffer
*/
void *
ioq_peek(ioq_t *ioq)
{
  return (ioq->buf + ioq->n);
}


/* ioq_seek()
**   discard len bytes from internal buffer
**   reposition ioq-p and update ioq->n accordingly
**   note:
**     len should be constrained:  current bytes > len >=0
*/
void
ioq_seek(ioq_t *ioq, size_t len)
{
  ioq->n += len;
  ioq->p -= len;
}


/* eof: ioq_get.c */
