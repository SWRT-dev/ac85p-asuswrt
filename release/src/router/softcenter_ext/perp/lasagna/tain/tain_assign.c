/* tain_assign.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.11.16
** ===
*/

#include "tain.h"

struct tain *
tain_assign(struct tain *t1, const struct tain *t2)
{
  t1->sec = t2->sec;
  t1->nsec = t2->nsec;

  return t1;
}


/* EOF (tain_assign.c) */
