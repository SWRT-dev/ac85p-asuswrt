/* tain_pause.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2008.02.11
** ===
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "tain.h"


/* nanosleep() wrapper: */
int
tain_pause(const struct tain *t, struct tain *rem)
{
  struct timespec t_req;
  struct timespec t_rem;
  int             err;

  t_req.tv_sec =  (time_t)t->sec;
  t_req.tv_nsec = (long)t->nsec;
  
  t_rem.tv_sec = 0;
  t_rem.tv_nsec = 0;

  err = nanosleep(&t_req, &t_rem);
  if(rem != NULL){
    rem->sec =  (uint64_t)t_rem.tv_sec;
    rem->nsec = (uint32_t)t_rem.tv_nsec;
  }

  return err;
}


/* EOF (tain_pause.c) */
