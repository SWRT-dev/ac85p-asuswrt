/* nfmt_uint32o_pad0.c
** stringify uint32_t numerical value
** pad with zeros '0' on left to fill width
** octal representation
** wcm, 2010.06.03 - 2011.02.01
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

char *
nfmt_uint32o_pad0(char *s, uint32_t n, size_t w)
{
  char    *s0 = s;
  size_t   len;

  len = nfmt_uint32o_(NULL, n);
  while(len < w){
      *s = '0';
      ++s;
      ++len;
  }
  
  s[nfmt_uint32o_(s, n)] = '\0';

  return s0;
}
          
/* eof: nfmt_uint32_pad0o.c */
