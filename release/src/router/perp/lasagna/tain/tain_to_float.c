/* tain_to_float.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include <stdlib.h>

#include "tain.h"


double
tain_to_float(struct tain *t)
{
  double  f = (double)t->sec + (t->nsec * 0.000000001);
  return f;
}


/* EOF (tain_to_float.c) */
