/* pollio.h
** pollio: lasagna wrapper for poll()
** wcm, 2010.12.29 - 2011.02.09
** ===
*/
#ifndef POLLIO_H
#define POLLIO_H

#include <poll.h>

/* pollio()
**   as poll(), ie:
**       if (msecs_max == 0):  don't block on poll(), return immediately
**       if (msecs_max == -1): poll() indefinitely (INFTIM)
**
**   additionally:
**       msecs_max is constrained to 100000 seconds (100000000 msecs)
**       if (msecs_rem != NULL), fill with unused time remaining from msecs_max:
**           if (msecs_max == -1) set msecs_rem = -1
**           if (msecs_max == 0) set msecs_rem = 0
**           if (msecs_max > 0):
**               if(poll() == 0), set msecs_rem = 0 (timer expired)
**               if(poll() > 0), set msecs_rem to:
**                   msecs_max - (stamp(after) - stamp(before))
**                   ie, "unused" msecs of msecs_max in poll()
**           note: in no case will (msecs_rem > msecs_max)? be true
**
**   returns as poll(), ie:
**     -1: poll() error, errno set (msecs_rem is not modified)
**      0: timeout reached, no events
**     >0: number of descriptors ready
*/
extern
int pollio(struct pollfd *pollv, nfds_t numfds, int msecs_max, int *msecs_rem);

#endif /* POLLIO_H */
/* eof: pollio.h */
