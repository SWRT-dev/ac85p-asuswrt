/* cstr_vcat_.c
** wcm, 2009.07.22 - 2009.11.16
** ===
*/

#include <stddef.h>
#include <stdarg.h>

#include "cstr.h"


size_t
cstr_vcat_(char *to, ...)
{
  va_list      args;
  const char  *to0 = to;
  const char  *from;

  va_start(args, to);

  for(;;){
      if(!(*to)) break; ++to;
      if(!(*to)) break; ++to;
      if(!(*to)) break; ++to;
      if(!(*to)) break; ++to;
  }

  from = va_arg(args, const char *);
  while(from){
      for(;;){
          if(!(*to = *from)) break; ++to; ++from;
          if(!(*to = *from)) break; ++to; ++from;
          if(!(*to = *from)) break; ++to; ++from;
          if(!(*to = *from)) break; ++to; ++from;
      }
      from = va_arg(args, const char *);
  }
  va_end(args);

  return (to - to0);
}

/* eof: cstr_vcat_.c */
