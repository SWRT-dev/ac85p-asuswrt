/* nfmt_uint32o.c
** stringify uint32_t numerical value
** result as octal
** wcm, 2004.04.19 - 2011.02.01
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
**   s[nfmt_uint32o_(s, n)] = '\0';
*/
size_t
nfmt_uint32o_(char *s, uint32_t n)
{
  size_t    len;
  uint32_t  q;

  len = 1; q = n;
  while (q > 7) { ++len; q /= 8; }
  if(s){
      s += len;
      do{
            *--s = '0' + (n % 8);
            n /= 8;
      }while(n); /* handles n == 0 */
  }

  return len;
}


/* nul terminate and return pointer: */
char *
nfmt_uint32o(char *s, uint32_t n)
{
  s[nfmt_uint32o_(s, n)] = '\0';
  return s;
}


/* eof: nfmt_uint32o.c */
