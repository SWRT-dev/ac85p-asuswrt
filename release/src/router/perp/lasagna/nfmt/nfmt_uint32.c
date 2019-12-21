/* nfmt_uint32.c
** stringify uint32_t numerical value
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

/*
** djb classic version:
** stringify decimal n into s
** s is _not_ nul terminated
** return number of characters in s
**
** customary usage to nul-terminate s:
**   char  s[NFMT_SIZE];
**   s[nfmt_uint32_(s, n)] = '\0';
*/
size_t
nfmt_uint32_(char *s, uint32_t n)
{
  size_t    len;
  uint32_t  q;

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
nfmt_uint32(char *s, uint32_t n)
{
  s[nfmt_uint32_(s, n)] = '\0';
  return s;
}


/* eof: nfmt_uint32.c */
