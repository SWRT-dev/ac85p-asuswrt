/* outvec_flush.c
** output using writev()
** wcm, 2008.01.04 - 2010.12.01
** ===
*/

#include <errno.h>
#include "outvec.h"

int
outvec_flush(struct outvec *vec)
{
  struct iovec  *v = vec->vec;
  size_t         nvec = vec->n;
  ssize_t        w = 0;
  size_t         i = 0;

  /*
  ** partial writev() handling adapted from algorithm published by
  ** Girish Venkatachalam
  */
  while(i < nvec){
      do{
          w = writev(vec->fd, &v[i], nvec - i);
      }while((w == -1) &&
             ((errno == EINTR) || (errno == EAGAIN)));

      if(w == -1){
          /* error (errno set by writev()): */
          return -1;
      } 

      /* check/handle for partial writes: */
      if(w > 0){
          while((i < nvec) && ((size_t)w >= v[i].iov_len)){
              w -= v[i].iov_len;
              ++i;
          }
          if(w > 0){
              v[i].iov_base = (char *)v[i].iov_base + w;
              v[i].iov_len -= w;
          }
      }
  }

  /* if here, all flushed: */
  vec->n = 0;
  return 0;
}


/* eof: outvec_flush.c */
