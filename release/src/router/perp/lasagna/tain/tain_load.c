/* tain_load.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include "tain.h"


struct tain *
tain_load(struct tain *t, uint64_t sec, uint32_t nsec)
{
  t->sec = sec;
  t->nsec = nsec;

  return t;
}


/* EOF (tain_load.c) */
