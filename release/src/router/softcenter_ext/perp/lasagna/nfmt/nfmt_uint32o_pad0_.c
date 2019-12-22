/* nfmt_uint32o_pad0_.c
** stringify uint32_t numerical value
** pad with zeros '0' on left to fill width
** octal representation
** wcm, 2010.06.03 - 2011.02.01
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

/*
** djb classic version:
** stringify decimal n into s of width w
** octal representation
** pad with zeros '0' on left to fill width
** s is _not_ nul terminated
** return number of characters in s
**
** customary usage to nul-terminate s:
**   char  s[NFMT_SIZE];
**   s[nfmt_uint32o_pad_(s, n)] = '\0';
**
** if s is NULL, return number of characters to format s
*/

size_t
nfmt_uint32o_pad0_(char *s, uint32_t n, size_t w)
{
  size_t   len;

  len = nfmt_uint32o_(NULL, n);
  while(len < w){
      if(s){ *s = '0'; ++s; }
      ++len;
  }
  if(s){
      nfmt_uint32o_(s, n);
  }

  return len;
}
          

/* eof: nfmt_uint32o_pad0_.c */
