/* cstr_dup.c
** wcm, 2004.04.19 - 2009.09.18
** ===
*/

#include <stddef.h>
#include <stdlib.h>

#include "cstr.h"

char *
cstr_dup(const char *s)
{
  char    *new = NULL;
  size_t   n;

  n = cstr_len(s);
  new = (char *)malloc((n + 1) * sizeof(char));
  if(!new){
      return NULL;
  }

  cstr_copy(new, s);
  return new;
}


/* eof: cstr_dup.c */
