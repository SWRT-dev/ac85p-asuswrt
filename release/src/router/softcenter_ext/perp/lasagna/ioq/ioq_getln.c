/* ioq_getln.c
** ioq read line operation
** wcm, 2004.04.21 - 2009.09.16
** ===
*/
/* lasagna: */
#include "buf.h"
#include "dynstr.h"

#include "ioq.h"

/*
** returns:
**  -1, error (io or malloc)
**   0, EOF
**   1, ok
*/

int
ioq_getln(ioq_t *q, dynstr_t *S)
{
  size_t    r, n;
  char     *b;
  int       found = 0;
  int       e;

  while(!found){
      r = ioq_feed(q);
      if(r <= 0){
          /* eof (0) or io error (-1) */
          return r;
      }

      b = (char *)ioq_PEEK(q);
      n = buf_ndx(b, r, '\n');
      if(n < r){
          /* '\n' found in b: */
          ++found;
          n += 1;
      } else {
          n = r;
      }
      /* if found, copy includes the '\n': */
      e = dynstr_putn(S, b, n);
      if(e == -1){
          /* dynstr malloc failure: */
          return -1;
      }
    
      ioq_SEEK(q, n);
  }

  /* success (found): */
  return 1;
}


/* eof: ioq_getln.c */
