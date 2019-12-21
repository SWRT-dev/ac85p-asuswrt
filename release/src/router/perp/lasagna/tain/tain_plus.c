/* tain_plus.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include "tain.h"

struct tain *
tain_plus(struct tain *t, const struct tain *a, const struct tain *b)
{
  t->sec = a->sec + b->sec;
  t->nsec = a->nsec + b->nsec;
  if(t->nsec > 999999999UL){
      t->nsec -= 1000000000UL;
      t->sec  += 1;
  }

  return t;
}


/* EOF (tain_plus.c) */
