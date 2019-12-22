/* tain_to_msecs.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include <stdlib.h>

#include "tain.h"


uint64_t
tain_to_msecs(struct tain *t)
{
  uint64_t msecs = (t->sec * 1000) + ((uint64_t)t->nsec / 1000000ULL);
  return msecs;
}


/* EOF (tain_to_msecs.c) */
