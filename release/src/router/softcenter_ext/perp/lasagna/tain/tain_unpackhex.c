/* tain_unpackhex.c
** tain: TAI timestamp with nanosecond precision
** wcm, 2009.08.04 - 2009.08.04
** ===
*/

#include "uchar.h"
#include "tain.h"


tain_t *
tain_unpackhex(tain_t *t, const char *s)
{
    uchar_t  tpack[TAIN_PACK_SIZE];
    uchar_t  u;
    int      i;

    for(i = 0; i < TAIN_PACK_SIZE; ++i){
        if((u = (uchar_t)s[i]) >= 'A'){
            /* bit op with 0xdf converts c if lowercase: */
            u = ((u & 0xdf) - 'A') + 10;
        } else {
            u -= '0';
        }
        tpack[i] = u << 4;

        if((u = (uchar_t)s[i + 1]) >= 'A'){
            u = ((u & 0xdf) - 'A') + 10;
        } else {
            u -= '0';
        }
        tpack[i] += u;
    }

    return tain_unpack(t, tpack);
}


/* EOF (tain_unpackhex.c) */
