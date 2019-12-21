/* pollio.c
** pollio: lasagna wrapper for poll()
** wcm, 2010.12.29 - 2011.02.01
** ===
*/

/* libc: */
#include <stdint.h>

/* lasagna: */
#include "tain.h"

/* pollio module: */
#include "pollio.h"


int
pollio(struct pollfd *pollv, nfds_t numfds, int msecs_max, int *msecs_rem)
{
  tain_t    before, after;
  uint64_t  diff;
  int       n;

  /* constrain msecs_max to 100000 seconds: */
  if(msecs_max > 100000000L) msecs_max = 100000000L;

  if((msecs_rem != NULL) && (msecs_max > 0)){
      tain_now(&before);
  }

  /* portability note: INFTIM is not defined on all systems */
  n = poll(pollv, numfds, (msecs_max < 0 ? -1 : msecs_max));

  if((msecs_rem != NULL) && (msecs_max > 0) && (n > 0)){
      tain_now(&after);
  }

  /* error: short-circuit return: */  
  if(n < 0) return n;

  /* non-blocking poll requested: */
  if(msecs_max == 0){
      if(msecs_rem != NULL) *msecs_rem = 0;
      return n;
  }

  /* infinite poll requested: */
  if(msecs_max == -1){
      if(msecs_rem != NULL) *msecs_rem = -1;
      return n;
  }

  /* remaining case:  msecs_max > 0 */

  /* if msecs_rem is non-NULL, compute the remaining time: */
  if(msecs_rem != NULL){
      if(n == 0){
          /* timer expired: */
          *msecs_rem = 0;
      } else {
          tain_now(&after);
          if(tain_less(&before, &after)){
              /* overwriting before with difference: */
              tain_minus(&before, &after, &before);
          } else {
              /* XXX, time going backwards! */
              tain_minus(&before, &before, &after);
          }
          /* convert difference to msecs: */
          diff = tain_to_msecs(&before);
          if(diff > (uint64_t)msecs_max) diff = (uint64_t)msecs_max;
          *msecs_rem = msecs_max - (int)diff;
      }
  }

  return n;
}

/* eof: pollio.c */
