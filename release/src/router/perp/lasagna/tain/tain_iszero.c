/* tain_iszero.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include "tain.h"

int
tain_iszero(const struct tain *t)
{
  return ((t->sec == 0) && (t->nsec == 0));
}


/* EOF (tain_iszero.c) */
