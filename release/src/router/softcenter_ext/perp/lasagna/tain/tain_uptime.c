/* tain_uptime.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2011.01.26 - 2011.01.26
** ===
*/

#include "tain.h"


uint64_t
tain_uptime(const struct tain *now, const struct tain *when)
{
    tain_t  temp;

    /* semantically, when happened before now: */
    if(tain_less(when, now))
        tain_minus(&temp, now, when);
    /* but occasionally, when updated since now: */
    else
        tain_minus(&temp, when, now);

    /* rounding: */
    if(temp.nsec > 500000000UL)
        temp.sec += 1;

    return  temp.sec;
}


/* EOF (tain_uptime.c) */
