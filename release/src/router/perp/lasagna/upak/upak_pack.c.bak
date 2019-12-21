/* upak_pack.c
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2010.06.02
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
upak_pack(uchar_t *buf, char *fmt, ...)
{
  va_list    args;
  uchar_t   *b = buf;
  char      *p;
  uint16_t   u16;
  uint32_t   u32;
  uint64_t   u64; 

  va_start(args, fmt);
  for(p = fmt; *p != '\0'; ++p){
      switch(*p){
      case 'b':
      case 'c': /* char (1 byte) */
          *b++ = (uchar_t) va_arg(args, int);
          break;
      case 's': /* short (2 byte) */
          u16 = (uint16_t) va_arg(args, int);
          *b++ = u16 & 255;
          *b++ = u16 >> 8;
          break;
      case 'd': /* long (4 byte) */
          u32 = va_arg(args, uint32_t);
          *b++ = u32 & 255; u32 >>= 8;
          *b++ = u32 & 255; u32 >>= 8;
          *b++ = u32 & 255; u32 >>= 8;
          *b++ = u32;
          break;
      case 'L': /* long long (8 byte) */
          u64 = va_arg(args, uint64_t);
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64 & 255; u64 >>= 8;
          *b++ = u64;
          break;
      default: /* unknown format character */
          va_end(args);
          errno = EINVAL;
          return -1;
      }
  }
  va_end(args);
  return b - buf;  
}


/* eof (upak_pack.c) */
