/* nfmt_uint64_pad_.c
** stringify uint64_t numerical value
** pad with space on left to fill width
** wcm, 2010.06.03 - 2010.06.03
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

/*
** djb classic version:
** stringify decimal n into s of width w
** pad with whitespace ' ' on left to fill width
** s is _not_ nul terminated
** return number of characters in s
**
** customary usage to nul-terminate s:
**   char  s[NFMT_SIZE];
**   s[nfmt_uint64_pad_(s, n)] = '\0';
**
** if s is NULL, return number of characters to format s
*/

size_t
nfmt_uint64_pad_(char *s, uint64_t n, size_t w)
{
  size_t   len;

  len = nfmt_uint64_(NULL, n);
  while(len < w){
      if(s){ *s = ' '; ++s; }
      ++len;
  }
  if(s){
      nfmt_uint64_(s, n);
  }

  return len;
}
          

/* eof: nfmt_uint64_pad_.c */
