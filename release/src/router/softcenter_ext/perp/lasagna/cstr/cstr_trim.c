/* cstr_trim.c
** wcm, 2009.09.16 - 2009.09.16
** ===
*/
/* stdlib: */
#include <ctype.h>

#include "cstr.h"

char *
cstr_trim(char *s)
{
  return cstr_ltrim(cstr_rtrim(s));
}

/* eof: cstr_trim.c */
