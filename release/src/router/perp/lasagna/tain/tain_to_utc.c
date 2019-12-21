/* tain_to_utc.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "tain.h"

static uint64_t tai_offset = TAIN_UTC_OFFSET;


time_t
tain_to_utc(struct tain *t)
{
   return (time_t)(t->sec - tai_offset);
}


/* EOF (tain_to_utc.c) */
