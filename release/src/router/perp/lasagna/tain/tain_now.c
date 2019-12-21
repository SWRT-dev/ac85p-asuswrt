/* tain_now.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2009.07.27
** ===
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "tain.h"

struct tain *
tain_now(struct tain *t)
{
  struct timeval now;

  gettimeofday(&now, NULL);
  tain_load_utc(t, now.tv_sec);
  t->nsec = (1000 * now.tv_usec) + 500;

  return t;
}


/* EOF (tain_now.c) */
