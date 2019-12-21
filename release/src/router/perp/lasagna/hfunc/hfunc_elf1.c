/* hfunc_elf1.c
** wcm, 2010.05.27 - 2010.06.07
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

#include "hfunc.h"

/* elf1()
** unix elf object file hash
** slight modification of pjw1
*/ 

/* hash function: */
uint32_t
hfunc_elf1(const uchar_t *key, size_t klen)
{
  uint32_t   h = 0;
  uint32_t   g;

  while(klen){
      h = (h << 4) + *key;
      g = h & 0xf0000000u;
      if(g != 0){
          h ^= g >> 24;
      }
      h &= ~g;
      ++key; --klen;
  }

  return h;
}

/* eof (hfunc_elf1.c) */
