/* nfmt_uint32x_pad.c
** stringify uint32_t numerical value as hexadecimal
** pad with space on left to fill width
** wcm, 2010.06.03 - 2010.11.01
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "nfmt.h"

char *
nfmt_uint32x_pad(char *s, uint32_t n, size_t w)
{
  char    *s0 = s;
  size_t   len;

  len = nfmt_uint32x_(NULL, n);
  while(len < w){
      *s = ' ';
      ++s;
      ++len;
  }
  
  s[nfmt_uint32x_(s, n)] = '\0';

  return s0;
}
          
/* eof: nfmt_uint32x_pad.c */
