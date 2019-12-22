/* tain_load_msecs.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>

#include "tain.h"

struct tain *
tain_load_msecs(struct tain *t, uint64_t msecs)
{
  t->sec = msecs / 1000;
  t->nsec = (msecs % 1000) * 1000000UL;

  return t;
}


/* EOF (tain_load_msecs.c) */
