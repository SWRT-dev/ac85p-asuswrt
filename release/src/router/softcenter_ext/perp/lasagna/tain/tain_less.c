/* tain_less.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include "tain.h"


int
tain_less(const struct tain *t1, const struct tain *t2)
{
  if(t1->sec != t2->sec){
      return (t1->sec < t2->sec ? 1 : 0);
  }

  return (t1->nsec < t2->nsec ? 1 : 0);
}


/* EOF (tain_less.c) */
