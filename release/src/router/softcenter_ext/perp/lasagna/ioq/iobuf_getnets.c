/*
** iobuf_getnets.c
** wcm, 2004.04.28 - 2004.04.29
** ===
*/
#include "iobuf.h"

#include "buf.h"
#include "dynbuf.h"


static
int
get_netslen(iobuf *ibuf, size_t *len, size_t max)
{
    char    c;
    size_t  n = 0;
    int     digits = 0;

/*
** returns length in len and:
**   -3, netstring format error
**   -1, io error
**    0, EOF
**    1, ok
*/

  for(;;){
      switch(iobuf_GETC(ibuf, &c)){
          case -1: return -1;  /* io error */
          case  0: return 0;   /* EOF */
          default: break;      /* ok */
      }
      if(c == ':'){
          if(digits){
              *len = n;
              return 1;
          }
          else break;
      }
      if((c < '0') || (c > '9')) break;
      n = 10 * n + (c - '0');
      if((digits && (n == 0)) || (n > max)) break;
      ++digits;
  }

  /* netstring format error: */
  return -3;
}


int iobuf_getnets(iobuf *ibuf, dynbuf *b, size_t max)
{
  int     r;
  size_t  len = 0; 
  char   *buf;
  char    c;

/*
** returns netstring in dynbuf:
**  -3, netstring format error
**  -1, error (io or malloc)
**   0, EOF
**   1, ok
*/

  switch(get_netslen(ibuf, &len, max)){
      case -3: return -3;
      case -1: return -1;
      case  0: return 0;
      default: break;
  }

  if(dynbuf_grow(b, len) != 0)
      return -1; /* malloc error */

  buf = dynbuf_BUF(b);
  while(len){
      r = iobuf_get(ibuf, buf, len);
      switch(r){
          case -1: return -1; /* io error */
          case  0: return -3; /* eof before complete */
          default: break;
      }
      dynbuf_LEN(b) += r;
      if(r == len) break;
      /* else: */
      buf += r;
      len -= r;
  }

  /* so far, so good; now comma? */
  switch(iobuf_GETC(ibuf, &c)){
      case -1: return -1;  /* io error */
      case  0: return -3;  /* eof before comma */
      default: break;
  }
  if(c != ',') return -3;

  /* success: */
  return 1;
}

/* that's all, folks! */
