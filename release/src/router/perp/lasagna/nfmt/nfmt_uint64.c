/* nfmt_uint64.c
** stringify uint64_t numerical value
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

size_t
nfmt_uint64_(char *s, uint64_t n)
{
  size_t    len;
  uint64_t  q;

  len = 1; q = n;
  while (q > 9) { ++len; q /= 10; }
  if(s){
      s += len;
      do{
            *--s = '0' + (n % 10);
            n /= 10;
      }while(n); /* handles n == 0 */
  }

  return len;
}


/* nul terminate and return pointer: */
char *
nfmt_uint64(char *s, uint64_t n)
{
  s[nfmt_uint64_(s, n)] = '\0';
  return s;
}

/* eof: nfmt_uint64.c */
