/* tain_minus.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include "tain.h"


struct tain *
tain_minus(struct tain *t, const struct tain *a, const struct tain *b)
{
  t->sec = a->sec - b->sec;
  if(a->nsec >= b->nsec){
      t->nsec = a->nsec - b->nsec;
  } else {
      t->sec -= 1;
      t->nsec = (1000000000UL + a->nsec) - b->nsec;
  }

  return t;
}


/* eof (tain_minus.c) */
