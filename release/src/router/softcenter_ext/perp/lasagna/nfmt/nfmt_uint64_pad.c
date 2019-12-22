/* nfmt_uint64_pad.c
** stringify uint64_t numerical value
** pad with space on left to fill width
** wcm, 2010.06.03 - 2010.06.03
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

char *
nfmt_uint64_pad(char *s, uint64_t n, size_t w)
{
  char    *s0 = s;
  size_t   len;

  len = nfmt_uint64_(NULL, n);
  while(len < w){
      *s = ' ';
      ++s;
      ++len;
  }
  
  s[nfmt_uint64_(s, n)] = '\0';

  return s0;
}
          
/* eof: nfmt_uint64_pad.c */
