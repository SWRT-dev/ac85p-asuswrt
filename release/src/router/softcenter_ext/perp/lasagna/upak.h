/* upak.h
** upak, portable storage for unsigned integers
** wcm, 2004.12.29 - 2012.08.04
** ===
*/
#ifndef UPAK_H
#define UPAK_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <unistd.h> /* ssize_t */

#include "uchar.h"

/*
**  upak:  portable storage format for unsigned integers
**  (an implementation of djb uint* utilities)
**
**  for n-byte unsigned integer u, find u0, u1, u2, u3, ..., such that
**
**      u = u0 + (u1*2^8) + (u2*2^16) + (u3*2^32) ...
**
**  then store u in buffer buf[0..(n-1)]:
**
**      buf[0] = u0, buf[1] = u1, buf[2] = u2, buf[3] = u3, ...
**
** 
**  example for 32-bit unsigned integer types (4 bytes):
**
**  uint32_t  u;
**  uchar_t   s[4];
**
**  upak32_pack(s, u);    // pack s from u
**  u = upak32_unpack(s); // unpack u from s
*/

/* upakNN_*() common interface:
** 
** upakNN_pack():
**   pack uintNN_t integer u into buffer b
**   return b
**
** upakNN_unpack():
**   return uintNN_t integer unpacked from buffer b
*/


/* upak16_*(), for uint16_t (caller supplies buf[2]): */
extern uchar_t * upak16_pack(uchar_t *buf, uint16_t u);
extern uint16_t upak16_unpack(const uchar_t *buf);
/* inline unpack: */
#define upak16_UNPACK(b) \
  (\
      (((uint16_t)(((uchar_t *)(b))[1])) << 8) + \
        (uint16_t)(((uchar_t *)(b))[0]) \
  )


/* upak24_*(), for uint32_t (caller supplies buf[3])
**   special notes:
**     pack/unpack unsigned integer in range [0..(2^24 - 1)]
**     no range checking in upak24_pack()
**     (caller must screen input)
*/
extern uchar_t * upak24_pack(uchar_t *buf, uint32_t u);
extern uint32_t upak24_unpack(const uchar_t *buf);
/* inline unpack: */
#define upak24_UNPACK(b) \
  (\
      (((uint32_t)(((uchar_t *)(b))[2])) << 16) + \
      (((uint32_t)(((uchar_t *)(b))[1])) << 8) + \
        (uint32_t)(((uchar_t *)(b))[0]) \
  )


/* upak32_*(), for uint32_t (caller supplies buf[4]): */
extern uchar_t * upak32_pack(uchar_t *buf, uint32_t u);
extern uint32_t upak32_unpack(const uchar_t *buf);
/* inline unpack: */
#define upak32_UNPACK(b) \
  (\
      (((uint32_t)(((uchar_t *)(b))[3])) << 24) + \
      (((uint32_t)(((uchar_t *)(b))[2])) << 16) + \
      (((uint32_t)(((uchar_t *)(b))[1])) << 8) + \
        (uint32_t)(((uchar_t *)(b))[0]) \
  )

/* upak48_*(), for uint64_t (caller supplies buf[6]): */
/*   special notes:
**     pack/unpack unsigned integer in range [0..(2^48 - 1)]
**     no range checking in upak48_pack()
**     (caller must screen input)
*/
extern uchar_t * upak48_pack(uchar_t *buf, uint64_t u);
extern uint64_t upak48_unpack(const uchar_t *buf);
/* inline unpack: */
#define upak48_UNPACK(b) \
  (\
      (((uint64_t)(((uchar_t *)(b))[5])) << 40) + \
      (((uint64_t)(((uchar_t *)(b))[4])) << 32) + \
      (((uint64_t)(((uchar_t *)(b))[3])) << 24) + \
      (((uint64_t)(((uchar_t *)(b))[2])) << 16) + \
      (((uint64_t)(((uchar_t *)(b))[1])) << 8) + \
        (uint64_t)(((uchar_t *)(b))[0]) \
  )


/* upak64_*(), for uint64_t (caller supplies buf[8]: */
extern uchar_t * upak64_pack(uchar_t *buf, uint64_t u);
extern uint64_t upak64_unpack(const uchar_t *buf);
/* inline unpack: */
#define upak64_UNPACK(b) \
  (\
      (((uint64_t)(((uchar_t *)(b))[7])) << 56) + \
      (((uint64_t)(((uchar_t *)(b))[6])) << 48) + \
      (((uint64_t)(((uchar_t *)(b))[5])) << 40) + \
      (((uint64_t)(((uchar_t *)(b))[4])) << 32) + \
      (((uint64_t)(((uchar_t *)(b))[3])) << 24) + \
      (((uint64_t)(((uchar_t *)(b))[2])) << 16) + \
      (((uint64_t)(((uchar_t *)(b))[1])) << 8) + \
        (uint64_t)(((uchar_t *)(b))[0]) \
  )


/* upak_*(buf, fmt, ...):
**   variable argument utilities:
**   under control of specification string in fmt
**   pack/unpack variable number of arguments to/from buf
**
**   characters interpreted in fmt:
**     'b'  1 byte  uchar_t  (ie, unsigned char)
**     's'  2 byte  uint16_t
**     'd'  4 byte  uint32_t
**     'L'  8 byte  uint64_t
**
**   return: number of bytes packed/unpacked from buf
*/

/* upak_pack()
**   pack variable uintNN_t arguments specified in fmt into buf
**   packed into successive position beginning from buf[0]
**
**   return:
**     >= 0: number of bytes packed into buf
**     -1  : error, errno = EINVAL (unknown format character in fmt)
**
**   note: caller must supply buf of sufficient size!
*/
extern ssize_t upak_pack(uchar_t *buf, const char *fmt, ...);

/* upak_vpack()
**   va_list version of upak_pack()
*/
extern ssize_t upak_vpack(uchar_t *b, const char *fmt, va_list args);

/* upak_unpack()
**   unpack from buf the variable number of packed integers specified in fmt
**   unpack into the matching number of variable integer arguments
**
**   return:
**     >= 0: number of bytes unpacked from buf
**     -1  : error, errno = EINVAL (unknown format character in fmt)
*/
extern ssize_t upak_unpack(uchar_t *buf, const char *fmt, ...);


#endif /* UPAK_H */
/* eof: upak.h */
