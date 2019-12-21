/* dynbuf_pack.c
** wcm, 2012.08.04 - 2012.08.04
** ===
*/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <unistd.h> /* ssize_t */
#include <errno.h>

#include "uchar.h"
#include "upak.h"

#include "dynbuf.h"

ssize_t
dynbuf_pack(struct dynbuf *D, const char *fmt, ...)
{
  va_list   args;
  size_t    need;
  char     *p;
  uchar_t  *b;
  ssize_t   w;

  /* grow space required in dynbuf: */
  p = (char *)fmt;
  need = 0;
  while(*p != '\0'){
      switch(*p){
      case('b'): /* fallthru: */
      case('c'): need += 1; break;
      case('s'): need += 2; break;
      case('d'): need += 4; break;
      case('L'): need += 8; break;
      default:
          errno = EINVAL;
          return -1;
      }
      ++p;
  }

  if(dynbuf_grow(D, need) == -1){
      return -1;
  }

  /* with need satisfied, append directly into buf: */
  b = (D->buf + D->p);

  va_start(args, fmt);
  w = upak_vpack(b, fmt, args);
  va_end(args);

  if((w == -1) || (w != (ssize_t)need)){
      errno = ENOMEM;
      return -1;
  }
  D->p += w;

  return w;
}


/* eof: dynbuf_pack.c */
