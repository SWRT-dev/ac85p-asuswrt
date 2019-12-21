/* cstr_chop.c
** wcm, 2010.06.15 - 2010.06.15
** ===
*/

#include "cstr.h"

char *
cstr_chop(char *s)
{
  char    *sN = s;

  while(*sN) ++sN;

  if(sN > s) --sN;

  if(*sN == '\n') *sN = '\0';
  
  return s;    
}

/* eof: cstr_chop.c */
