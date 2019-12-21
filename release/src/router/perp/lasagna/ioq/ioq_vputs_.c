/* ioq_vputs_.c
** wcm, 2009.08.02 - 2009.08.02
** ===
*/

#include <stddef.h>
#include <stdarg.h>

#include "ioq.h"

int
ioq_vputs_(ioq_t *ioq, const char *s0, ...)
{
  va_list      args;
  const char  *s = s0;
  int          e;

  va_start(args, s0);
  do{
      if((e = ioq_puts(ioq, s)) == -1)
          break;
      s = va_arg(args, const char*);
  }while(s != NULL);
  va_end(args);

  return e;
}


/* eof: ioq_vputs_.c */
