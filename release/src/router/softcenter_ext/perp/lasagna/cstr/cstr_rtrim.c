/* cstr_rtrim.c
** wcm, 2009.09.16 - 2009.09.16
** ===
*/
/* stdlib: */
#include <ctype.h>

#include "cstr.h"

char *
cstr_rtrim(char *s)
{
  char    *sN = s;

  while(*sN)
    ++sN;

  if(sN > s)
    --sN;
  
  while((sN > s) && isspace((int)(unsigned char)*sN)){
      *sN-- = '\0';
  }
  
  return s;    
}

/* eof: cstr_rtrim.c */
