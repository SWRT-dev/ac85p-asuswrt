/* upak_pack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2012.08.04
** ===
*/
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <unistd.h> /* ssize_t */
#include <errno.h>

#include "uchar.h"
#include "upak.h"


ssize_t
upak_pack(uchar_t *buf, const char *fmt, ...)
{
  va_list    args;
  ssize_t    w;

  va_start(args, fmt);
  w = upak_vpack(buf, fmt, args);
  va_end(args);

  return w;
}


/* eof (upak_pack.c) */
