/* cstr_ltrim.c
** wcm, 2009.09.16 - 2009.09.16
** ===
*/
/* stdlib: */
#include <ctype.h>

#include "cstr.h"

char *
cstr_ltrim(char *s)
{
  char  *sX = s;
  char  *sN = s;

  /* scan sX beyond leading whitespace: */
  while(*sX && isspace((int)(unsigned char)*sX))
      ++sX;

  if(sX != s){
      while(*sX){
          *sN++ = *sX++;
      }
      *sN = '\0';
  }

  return s;
}


/* eof: cstr_ltrim.c */
