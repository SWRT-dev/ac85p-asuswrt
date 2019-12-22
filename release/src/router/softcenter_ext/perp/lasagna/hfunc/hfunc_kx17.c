/* hfunc_kx17.c
** wcm, 2010.05.27 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* kx17
** Peter Kankowski[*6] "x17"
** much like ghfa with init=0, mult=17
** except: a constant of 32 is subtracted from each key byte before mixing
**
** theory:
** <quote>
**   x17 hash function subtracts a space from each letter to cut off
**   the control characters in the range 0x00..0x1F. If the hash keys
**   are long and contain only Latin letters and numbers, the letters
**   will be less frequently shifted out, and the overall number of
**   collisions will be lower. You can even subtract 'A' when you know
**   that the keys will be only English words.
** </quote>
** see Peter Kanowski's website noted in hfunc.h [*6]   
**
** note:
** this function could be parameterized for the subtraction constant,
** but isn't
**
** this version uses bit shift/add operation for multiplier
** ie:  (h * 17) == ((h << 4) + h)
*/

uint32_t
hfunc_kx17(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;

  while(klen){
      h = ((h << 4) + h) + (*key - ' ');
      ++key;
      --klen;
  }

  return h;
}

/* eof (hfunc_kx17.c) */
