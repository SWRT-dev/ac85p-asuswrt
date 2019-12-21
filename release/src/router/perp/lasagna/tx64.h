/* tx64.h
** tx64: base 64 encoding module
** wcm, 2009.07.19 - 2009.07.20
** ===
*/
#ifndef TX64_H
#define TX64_H 1

#include <stddef.h>


/* base64 encoding utility */

#define  TX64_PAD    0x0
#define  TX64_NOPAD  0x1

/* tx64_encode():
**   if dest is NULL:
**       return number of bytes required for dest to encode src
**   if dest is non-NULL:
**       encode src of length len into dest using encoding vector vec
**       return number of bytes written into dest
**       caller responsible for providing dest buffer of sufficient size 
**   vec:
**       caller supplies an encoding vector of 65 characters
**       vec[0 .. 63] are base 64 encoding characters
**       vec[64] is padding character
**   flags:
**       TX64_NOPAD: do not pad encoding for src with len%3 != 0
**       default is to pad encoding
*/
extern
size_t tx64_encode
(
  char        *dest,
  const char  *src,
  size_t       len,
  const char  *vec,
  int          flags
);


/* standard base64 encoding vector: */
extern const char base64_vec[64];


#endif /* TX64_H */
