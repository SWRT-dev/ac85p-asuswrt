/* tain_load_utc.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "tain.h"

static uint64_t tai_offset = TAIN_UTC_OFFSET;

struct tain *
tain_load_utc(struct tain *t, time_t u)
{
  t->sec = tai_offset + (uint64_t)u;
  t->nsec = 0;

  return t;
}


/* EOF (tain_load_utc.c) */
