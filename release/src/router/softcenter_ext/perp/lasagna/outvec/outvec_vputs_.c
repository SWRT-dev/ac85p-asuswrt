/* outvec_vputs_.c
** output using writev()
** wcm, 2010.11.27 - 2010.12.01
** ===
*/

#include <stdarg.h>
#include "outvec.h"

int
outvec_vputs_(struct outvec *vec, const char *s0, ...)
{
  va_list      ap;
  const char  *s = s0;

  va_start(ap, s0);
  while(s != NULL){
      if(outvec_puts(vec, s) == -1){
          va_end(ap);
          return -1;
      }
      s = va_arg(ap, const char *);
  }
  va_end(ap);

  return (vec->flushme ? outvec_flush(vec) : 0);
}


/* eof: outvec_vputs_.c */
