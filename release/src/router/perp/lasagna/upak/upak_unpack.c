/* upak_unpack.c
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
upak_unpack(uchar_t *buf, const char *fmt, ...)
{
  va_list    args;
  uchar_t   *b = buf;
  char      *p;
  uint8_t   *u8p;
  uint16_t  *u16p;
  uint32_t  *u32p;
  uint64_t  *u64p; 

  va_start(args, fmt);
  for(p = (char *)fmt; *p != '\0'; ++p){
      switch(*p){
      case 'b':
      case 'c': /* char (1 byte) */
          u8p = va_arg(args, uint8_t*);
          if(u8p != NULL){
              *u8p = *b;
          }
          b += 1;
          break;
      case 's': /* short (2 byte) */
          u16p = va_arg(args, uint16_t*);
          if(u16p != NULL){
              *u16p =  b[1]; *u16p <<= 8;
              *u16p += b[0];
          }
          b += 2;
          break;
      case 'd': /* long (4 byte) */
          u32p = va_arg(args, uint32_t*);
          if(u32p != NULL){
              *u32p =  b[3]; *u32p <<= 8;
              *u32p += b[2]; *u32p <<= 8;
              *u32p += b[1]; *u32p <<= 8;
              *u32p += b[0];
          }
          b += 4;
          break;
      case 'L': /* long long (8 byte) */
          u64p = va_arg(args, uint64_t*);
          if(u64p != NULL){
              *u64p =  b[7]; *u64p <<= 8;
              *u64p += b[6]; *u64p <<= 8;
              *u64p += b[5]; *u64p <<= 8;
              *u64p += b[4]; *u64p <<= 8;
              *u64p += b[3]; *u64p <<= 8;
              *u64p += b[2]; *u64p <<= 8;
              *u64p += b[1]; *u64p <<= 8;
              *u64p += b[0];
          }
          b += 8;
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


/* eof: upak_unpack.c */
