/* upak_vpack.c
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

/* upak_vpack():
**   * added this va_list function to support dynbuf_pack()
**   * converted original upak_pack() to a wrapper for this function
**
** 2012.08.04
*/

ssize_t
upak_vpack(uchar_t *buf, const char *fmt, va_list args)
{
  uchar_t   *b = buf;
  char      *p;
  uint16_t   u16;
  uint32_t   u32;
  uint64_t   u64; 

  for(p = (char *)fmt; *p != '\0'; ++p){
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
          errno = EINVAL;
          return -1;
      }
  }

  return (ssize_t)(b - buf);  
}


/* eof: upak_vpack.c */
