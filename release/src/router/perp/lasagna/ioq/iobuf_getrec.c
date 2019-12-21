/*
** iobuf_getrec.c
** wcm, 2004.04.28 - 2004.04.29
** ===
*/
#include "iobuf.h"

#include "buf.h"
#include "dynbuf.h"

int iobuf_getrec(iobuf *ibuf, dynbuf *b, int sep)
{
  int    r;
  int    n;
  char  *x;

/*
** returns:
**  -1, error (io or malloc)
**   0, EOF
**   1, ok
*/

  for(;;){
      r = iobuf_feed(ibuf);
      if(r <= 0) return r;   /* EOF (0) or io error (-1) */

      x = (char *)iobuf_PEEK(ibuf);
      n = buf_ndx(x, r, sep);
      if(n < r){ /* sep found */
          if(dynbuf_putbuf(b, x, n) == 0) { /* ok */
              /* strip contents out of ibuf */
              iobuf_SEEK(ibuf, (n+1));
              /* success: */
              return 1;
          }
          else /* dynbuf malloc failure: */
              return -1;
      }
      /* else: */
      if(dynbuf_grow(b, r) != 0)
          return -1;
      dynbuf_LEN(b) += iobuf_get(ibuf, dynbuf_BUF(b)+dynbuf_LEN(b), r);
  }

  /* not here: */
  return -1;
}

/* that's all, folks! */
