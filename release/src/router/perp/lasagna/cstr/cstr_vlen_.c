/* cstr_vlen_.c
** wcm, 2009.07.22 - 2009.07.22
** ===
*/

#include <stddef.h>
#include <stdarg.h>

#include "cstr.h"


size_t
cstr_vlen_(const char *s0, ...)
{
  va_list      args;
  const char  *s = s0;
  size_t       n = 0;

  va_start(args, s0);
  do{
      n += cstr_len(s);
      s = va_arg(args, const char*);
  }while(s != NULL);
  va_end(args);

  return n;
}

/* eof: cstr_vlen_.c */
