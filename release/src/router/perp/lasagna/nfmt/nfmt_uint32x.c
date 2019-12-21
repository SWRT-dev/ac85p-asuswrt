/* nfmt_uint32x.c
** stringify uint32_t numerical value into hexadecimal
** wcm, 2004.04.19 - 2010.10.29
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"


/* hexify 4-bit value in x: */
#define NFMT_HEXIFY(x)  \
  ( ((x) < 10) ? \
    ((x) + '0') : \
    ((x) - 10 + 'a') )

/*
** djb classic version:
** stringify decimal n into s, hexadecimal representation
** s is _not_ nul terminated
** return number of characters in s
**
** customary usage to nul-terminate s:
**   char  s[NFMT_SIZE];
**   s[nfmt_uint32x_(s, n)] = '\0';
*/
size_t
nfmt_uint32x_(char *s, uint32_t n)
{
  size_t    len;
  uint32_t  q;

  len = 1; q = n;
  /* len used in s is number of 4-bit quants in n: */
  while (q > 16) { ++len; q >>= 4; }
  if(s){
      /* fill s from end: */
      s += len;
      do{
            *--s = NFMT_HEXIFY(n & 15);
            n >>= 4;
      }while(n); /* handles n == 0 */
  }

  return len;
}


/* nul terminate and return pointer: */
char *
nfmt_uint32x(char *s, uint32_t n)
{
  s[nfmt_uint32x_(s, n)] = '\0';
  return s;
}


/* eof: nfmt_uint32x.c */
