/* hfunc_sfh1.c
** "super fast hash"
** hash algorithm by paul hsieh
** lasagna implementation
** wcm, 2010.06.05 - 2010.06.06
** ===
*/

#include <stddef.h>
#include <stdint.h>

/* lasagna: */
#include "uchar.h"
#include "upak.h"

#include "hfunc.h"

/*
** The following source code is derived from original sources published
** under a separate license.
** 
** First, the permission granted to use the original source code under a
** specific license:
** 
** <quote>
**   For the specific coverage of raw source code (only) obtained from
**   this website, you have the option of using the old-style BSD license
**   to use the code instead of other the licenses. This option has been
**   provided for people who can't figure out what I talking about with
**   my derivative license, or who are using a old-style BSD compatible
**   license.
** </quote>
**   
** Second, the license itself:
** 
** <quote>
**   Paul Hsieh OLD BSD license
**   
**   Copyright (c) 2010, Paul Hsieh All rights reserved.
**   
**   Redistribution and use in source and binary forms, with or without
**   modification, are permitted provided that the following conditions
**   are met:
**   
**       * Redistributions of source code must retain the above copyright
**         notice, this list of conditions and the following disclaimer.
**   
**       * Redistributions in binary form must reproduce the above
**         copyright notice, this list of conditions and the following
**         disclaimer in the documentation and/or other materials provided
**         with the distribution.
**   
**       * My name, Paul Hsieh, and the names of any other contributors
**         to the code use may be used to endorse or promote products
**         derived from this software without specific prior written
**         permission.
**   
**   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
**   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
**   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
**   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
**   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
**   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
**   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
**   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
**   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
**   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
**   POSSIBILITY OF SUCH DAMAGE.
** </quote> 
**
**
** The quotes above extracted on 2010.06.05 from the web document:
**
**    http://www.azillionmonkeys.com/qed/weblicense.html
*/


/*
** The following source derived, modified and adapted for lasagna from
** original sources published on the web document at:
**
**   http://www.azillionmonkeys.com/qed/hash.html
**
** wcm, 2010.06.05
*/


/* hash function: */
uint32_t
hfunc_sfh1(const uchar_t *key, size_t klen)
{
  uint32_t   h = klen;
  uint32_t   tmp;

  /* main loop processes 2 16-bit chunks of key at a time
  ** ie, 4 bytes per loop
  **
  ** notes:
  **   loop sentinal ensures at least 4 bytes remain in key
  **   upak16_UNPACK() on key is consistent across architectures 
  */
  while(klen >= 4){
      h += (uint32_t)upak16_UNPACK(key);
      tmp = (((uint32_t)upak16_UNPACK(key + 2)) << 11) ^ h;
      h = (h << 16) ^ tmp;
      h += h >> 11;
      key += 4;
      klen -= 4;
  }

  /* process any klen remaining bytes: */
  switch(klen){
  case 3:
      h += (uint32_t)upak16_UNPACK(key);
      h ^= h << 16;
      h ^= key[2] << 18;
      h += h >> 11;
      break;
  case 2:
      h += (uint32_t)upak16_UNPACK(key);
      h ^= h << 11;
      h += h >> 17;
      break;
  case 1:
      h += *key;
      h ^= h << 10;
      h += h >> 1;
  }

  /* avalanche final 127 bits: */
  h ^= h << 3;
  h += h >> 5;
  h ^= h << 4;
  h += h >> 17;
  h ^= h << 25;
  h += h >> 6;

  return h;
}

/* eof (hfunc_sfh1.c) */
